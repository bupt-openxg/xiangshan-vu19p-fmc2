#!/usr/bin/env python3
import argparse
from pathlib import Path

from gen_fft_perf_reports import PerfMetrics, parse_report


def parse_steady_state(path: Path) -> tuple[int, int]:
    warmup = 0
    measured = 0
    for line in path.read_text(errors="replace").splitlines():
        if not line.startswith("FFT_STEADY_STATE "):
            continue
        parts = {}
        for item in line.split()[1:]:
            key, value = item.split("=", 1)
            parts[key] = value
        warmup = int(parts.get("warmup", "0"))
        measured = int(parts.get("measured_repeat", "0"))
        break
    return warmup, measured


def fmt_int(value: int) -> str:
    return f"{value:,}"


def fmt_float(value: float, digits: int = 2) -> str:
    return f"{value:.{digits}f}"


def fmt_pct(value: float) -> str:
    return f"{value * 100:.2f}%"


def pct_delta(base: float, other: float) -> float:
    if base == 0:
        return 0.0
    return (other - base) / base


def find_report(run_dir: Path, version: str, points: int, repeat: int, implementation: str) -> Path:
    sim_dir = run_dir / f"n{points}" / version / "sim" / implementation
    matches = sorted(sim_dir.glob(f"n{points}-r{repeat}-idft0-*.report.txt"))
    if len(matches) != 1:
        raise FileNotFoundError(
            f"expected exactly one report under {sim_dir} for n{points} r{repeat}, found {len(matches)}"
        )
    return matches[0]


def hpm_rows(metrics: PerfMetrics) -> list[tuple[str, str, int, float]]:
    if metrics.roi is None:
        return []
    rows = []
    repeat = metrics.roi.repeat if metrics.roi.repeat else 1
    for (group, event), total in sorted(metrics.roi.hpm.items()):
        rows.append((group, event, total, total / repeat))
    return rows


def render_hpm_table(metrics: PerfMetrics) -> list[str]:
    rows = hpm_rows(metrics)
    if not rows:
        return ["- 该模式未采到 ROI HPM 事件。", ""]

    lines = [
        "| 分组 | 事件 | ROI 总量 | 平均/次 |",
        "| --- | --- | ---: | ---: |",
    ]
    for group, event, total, per_fft in rows:
        lines.append(f"| `{group}` | `{event}` | {fmt_int(total)} | {fmt_float(per_fft)} |")
    lines.append("")
    return lines


def render_report(
    version: str,
    points: int,
    repeat: int,
    publish_path: Path,
    publish_metrics: PerfMetrics,
    diag_path: Path,
    diag_metrics: PerfMetrics,
) -> str:
    if publish_metrics.roi is None or diag_metrics.roi is None:
        raise ValueError("dual-mode report requires ROI data in both publish and diagnose runs")

    warmup, measured = parse_steady_state(publish_path)
    publish_avg_cycle = publish_metrics.roi.cycle / publish_metrics.roi.repeat
    diag_avg_cycle = diag_metrics.roi.cycle / diag_metrics.roi.repeat
    publish_avg_inst = publish_metrics.roi.instret / publish_metrics.roi.repeat
    diag_avg_inst = diag_metrics.roi.instret / diag_metrics.roi.repeat
    cycle_delta = pct_delta(publish_avg_cycle, diag_avg_cycle)
    inst_delta = pct_delta(publish_avg_inst, diag_avg_inst)

    perturbation_judgement = "较小"
    if abs(cycle_delta) > 0.03:
        perturbation_judgement = "偏大"
    elif abs(cycle_delta) > 0.01:
        perturbation_judgement = "可见"

    lines = [
        f"# {version.upper()} DFT{points} 双模式测量分析",
        "",
        "- 本页是双模式测量框架的试跑分析，不覆盖现有 mainline 性能页。",
        "- `publish` 版只作为正式结果发表口径；`diagnose` 版只用于瓶颈定位与调优参考。",
        "- 本页只用 ROI `cycle / instret / IPC / HPM` 讨论 kernel；不再用 whole-program `VFEX/VLSU` 类统计去反推 steady-state 根因。",
        "",
        "## 1. 运行配置",
        "",
        f"- 点数：`{points}`",
        f"- measured repeat：`{measured or repeat}`",
        f"- warmup repeat：`{warmup}`",
        f"- publish 报告：`{publish_path}`",
        f"- diagnose 报告：`{diag_path}`",
        "",
        "## 2. publish 与 diagnose 扰动对比",
        "",
        "| 模式 | ROI instret | ROI cycle | 平均指令/次 | 平均周期/次 | ROI IPC | ROI CPI |",
        "| --- | ---: | ---: | ---: | ---: | ---: | ---: |",
        (
            f"| `publish` | {fmt_int(publish_metrics.roi.instret)} | {fmt_int(publish_metrics.roi.cycle)} | "
            f"{fmt_float(publish_avg_inst)} | {fmt_float(publish_avg_cycle)} | "
            f"{fmt_float(publish_metrics.primary_ipc(), 6)} | {fmt_float(publish_metrics.primary_cpi(), 6)} |"
        ),
        (
            f"| `diagnose` | {fmt_int(diag_metrics.roi.instret)} | {fmt_int(diag_metrics.roi.cycle)} | "
            f"{fmt_float(diag_avg_inst)} | {fmt_float(diag_avg_cycle)} | "
            f"{fmt_float(diag_metrics.primary_ipc(), 6)} | {fmt_float(diag_metrics.primary_cpi(), 6)} |"
        ),
        "",
        f"- `diagnose` 相对 `publish` 的 `平均周期/次` 变化是 `{fmt_pct(cycle_delta)}`，属于 `{perturbation_judgement}` 扰动。",
        f"- `平均指令/次` 变化是 `{fmt_pct(inst_delta)}`；如果这里只是轻微漂移，说明额外扰动主要来自测量框架边界，而不是 kernel 工作量被改写。",
        "",
        "## 3. publish 版可发表主口径",
        "",
    ]
    lines.extend(render_hpm_table(publish_metrics))
    lines.extend(
        [
            "## 4. diagnose 版附加 ROI HPM",
            "",
        ]
    )
    lines.extend(render_hpm_table(diag_metrics))
    lines.extend(
        [
            "## 5. 试跑结论",
            "",
            f"- 对 `N={points}` 这个点，双模式框架已经能把“正式口径”和“诊断口径”拆开，且 `diagnose` 对 `publish` 的 ROI 扰动为 `{fmt_pct(cycle_delta)}`。",
            "- `publish` 版继续保持小而稳的主指标；`diagnose` 版则把 backend 相关的 `rob / branch / exec stall / memstall` 补齐到 ROI 口径里。",
            "- 由于本轮仍然不改 RTL，诊断版还拿不到 `VFEX0/VLSU` 这类 cluster 级 ROI 事件；它的定位能力上限就是“backend/topdown/ROB 级”，不能越级下结论。",
            "",
        ]
    )
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--run-dir", required=True)
    parser.add_argument("--version", required=True)
    parser.add_argument("--points", required=True, type=int)
    parser.add_argument("--repeat", required=True, type=int)
    parser.add_argument("--publish-impl", default="publish")
    parser.add_argument("--diag-impl", default="diag-backend")
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    run_dir = Path(args.run_dir).resolve()
    publish_path = find_report(run_dir, args.version, args.points, args.repeat, args.publish_impl)
    diag_path = find_report(run_dir, args.version, args.points, args.repeat, args.diag_impl)
    publish_metrics = parse_report(publish_path)
    diag_metrics = parse_report(diag_path)

    output_path = Path(args.output).resolve()
    output_path.write_text(
        render_report(
            version=args.version,
            points=args.points,
            repeat=args.repeat,
            publish_path=publish_path,
            publish_metrics=publish_metrics,
            diag_path=diag_path,
            diag_metrics=diag_metrics,
        )
        + "\n"
    )
    print(f"Wrote {output_path}")


if __name__ == "__main__":
    main()
