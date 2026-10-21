#!/usr/bin/env python3
import argparse
from dataclasses import dataclass
from pathlib import Path

from gen_fft_perf_reports import REPORT_NAME_RE, parse_report


CANONICAL_REPEAT = {
    16: 30,
    64: 10,
    128: 10,
    256: 10,
    512: 10,
    1024: 5,
    2048: 3,
    4096: 3,
}

PASS_THRESHOLD_PCT = 1.0


@dataclass
class Sample:
    version: str
    impl: str
    points: int
    repeat: int
    warmup: int
    roi_cycle: int
    roi_instret: int
    avg_cycle: float
    avg_instret: float
    roi_ipc: float
    whole_cycle: int
    whole_instr: int
    report_path: Path


@dataclass
class Verdict:
    version: str
    points: int
    repeats: list[int]
    canonical_repeat: int
    canonical_sample: Sample
    compare_sample: Sample | None
    compare_delta_cycle_pct: float | None
    compare_delta_inst_pct: float | None
    secondary_sample: Sample | None
    secondary_delta_cycle_pct: float | None
    secondary_delta_inst_pct: float | None
    status: str
    note: str


def fmt_int(value: int) -> str:
    return f"{value:,}"


def fmt_float(value: float, digits: int = 2) -> str:
    return f"{value:,.{digits}f}"


def fmt_pct(value: float | None) -> str:
    if value is None:
        return "-"
    return f"{value:.3f}%"


def pct_delta(base: float, other: float) -> float:
    if base == 0:
        return 0.0
    return (other - base) / base * 100.0


def load_samples(run_dir: Path) -> list[Sample]:
    samples: list[Sample] = []
    for path in sorted(run_dir.glob("n*/**/sim/*/*.report.txt")):
        rel = path.relative_to(run_dir)
        if len(rel.parts) != 5:
            continue

        report_name = path.name
        match = REPORT_NAME_RE.match(report_name)
        if not match:
            continue

        version = rel.parts[1]
        impl = rel.parts[3]
        points = int(match.group("points"))
        repeat = int(match.group("repeat"))
        perf = parse_report(path)
        if perf.roi is None:
            raise ValueError(f"missing FFT_ROI summary in {path}")

        warmup = 0
        for line in path.read_text(errors="replace").splitlines():
            if line.startswith("FFT_STEADY_STATE "):
                parts = dict(item.split("=") for item in line.split()[1:])
                warmup = int(parts.get("warmup", "0"))
                break

        samples.append(
            Sample(
                version=version,
                impl=impl,
                points=points,
                repeat=repeat,
                warmup=warmup,
                roi_cycle=perf.roi.cycle,
                roi_instret=perf.roi.instret,
                avg_cycle=perf.roi.cycle / perf.roi.repeat,
                avg_instret=perf.roi.instret / perf.roi.repeat,
                roi_ipc=perf.roi.instret / perf.roi.cycle if perf.roi.cycle else 0.0,
                whole_cycle=perf.cycle,
                whole_instr=perf.instr,
                report_path=path,
            )
        )
    return samples


def build_verdict(version: str, points: int, samples: list[Sample]) -> Verdict:
    ordered = sorted(samples, key=lambda item: item.repeat)
    repeats = [item.repeat for item in ordered]
    canonical_repeat = CANONICAL_REPEAT[points]
    sample_by_repeat = {item.repeat: item for item in ordered}
    canonical_sample = sample_by_repeat[canonical_repeat]

    lower = max((item for item in ordered if item.repeat < canonical_repeat), key=lambda item: item.repeat, default=None)
    higher = min((item for item in ordered if item.repeat > canonical_repeat), key=lambda item: item.repeat, default=None)

    compare_sample = higher if higher is not None else lower
    compare_delta_cycle_pct = None
    compare_delta_inst_pct = None
    if compare_sample is not None:
        compare_delta_cycle_pct = abs(pct_delta(canonical_sample.avg_cycle, compare_sample.avg_cycle))
        compare_delta_inst_pct = abs(pct_delta(canonical_sample.avg_instret, compare_sample.avg_instret))

    secondary_sample = None
    secondary_delta_cycle_pct = None
    secondary_delta_inst_pct = None
    if lower is not None and higher is not None:
        secondary_sample = lower
        secondary_delta_cycle_pct = abs(pct_delta(lower.avg_cycle, canonical_sample.avg_cycle))
        secondary_delta_inst_pct = abs(pct_delta(lower.avg_instret, canonical_sample.avg_instret))

    if compare_sample is None:
        status = "matrix-incomplete"
        note = "当前矩阵缺少 canonical 邻接档位，无法判定。"
    elif compare_delta_cycle_pct is not None and compare_delta_cycle_pct <= PASS_THRESHOLD_PCT:
        status = "pass"
        note = (
            f"canonical r{canonical_repeat} 与邻接更高档 r{compare_sample.repeat} 的 "
            f"平均周期/次差异为 {compare_delta_cycle_pct:.3f}%，已落入 1% 内。"
            if compare_sample.repeat > canonical_repeat
            else f"当前仅有较低邻接档 r{compare_sample.repeat}，差异为 {compare_delta_cycle_pct:.3f}%，暂视作通过。"
        )
    else:
        status = "fail"
        if compare_sample.repeat > canonical_repeat:
            note = (
                f"canonical r{canonical_repeat} 到 r{compare_sample.repeat} 的平均周期/次仍变化 "
                f"{compare_delta_cycle_pct:.3f}%，未进入 1% 收敛带。"
            )
        else:
            note = (
                f"当前只有较低邻接档 r{compare_sample.repeat}，与 canonical r{canonical_repeat} 的差异是 "
                f"{compare_delta_cycle_pct:.3f}%，还不能把 canonical 视作稳定。"
            )

    return Verdict(
        version=version,
        points=points,
        repeats=repeats,
        canonical_repeat=canonical_repeat,
        canonical_sample=canonical_sample,
        compare_sample=compare_sample,
        compare_delta_cycle_pct=compare_delta_cycle_pct,
        compare_delta_inst_pct=compare_delta_inst_pct,
        secondary_sample=secondary_sample,
        secondary_delta_cycle_pct=secondary_delta_cycle_pct,
        secondary_delta_inst_pct=secondary_delta_inst_pct,
        status=status,
        note=note,
    )


def render_tsv(verdicts: list[Verdict]) -> str:
    lines = [
        "\t".join(
            [
                "version",
                "points",
                "repeats",
                "canonical_repeat",
                "canonical_avg_cycle",
                "canonical_avg_instret",
                "canonical_roi_ipc",
                "compare_repeat",
                "compare_avg_cycle",
                "delta_cycle_pct",
                "delta_inst_pct",
                "status",
                "note",
                "canonical_report",
            ]
        )
    ]
    for verdict in verdicts:
        compare = verdict.compare_sample
        lines.append(
            "\t".join(
                [
                    verdict.version,
                    str(verdict.points),
                    ",".join(str(item) for item in verdict.repeats),
                    str(verdict.canonical_repeat),
                    f"{verdict.canonical_sample.avg_cycle:.6f}",
                    f"{verdict.canonical_sample.avg_instret:.6f}",
                    f"{verdict.canonical_sample.roi_ipc:.6f}",
                    str(compare.repeat) if compare is not None else "",
                    f"{compare.avg_cycle:.6f}" if compare is not None else "",
                    f"{verdict.compare_delta_cycle_pct:.6f}" if verdict.compare_delta_cycle_pct is not None else "",
                    f"{verdict.compare_delta_inst_pct:.6f}" if verdict.compare_delta_inst_pct is not None else "",
                    verdict.status,
                    verdict.note,
                    str(verdict.canonical_sample.report_path),
                ]
            )
        )
    return "\n".join(lines) + "\n"


def render_group_table(verdicts: list[Verdict]) -> list[str]:
    lines = [
        "| 点数 | 已测 repeats | canonical | canonical 平均周期/次 | 对比档 | 对比平均周期/次 | cycle 差异 | inst 差异 | 判定 |",
        "| ---: | --- | ---: | ---: | ---: | ---: | ---: | ---: | --- |",
    ]
    for verdict in sorted(verdicts, key=lambda item: item.points):
        compare = verdict.compare_sample
        lines.append(
            "| "
            + " | ".join(
                [
                    str(verdict.points),
                    "`" + "/".join(str(item) for item in verdict.repeats) + "`",
                    str(verdict.canonical_repeat),
                    fmt_float(verdict.canonical_sample.avg_cycle),
                    str(compare.repeat) if compare is not None else "-",
                    fmt_float(compare.avg_cycle) if compare is not None else "-",
                    fmt_pct(verdict.compare_delta_cycle_pct),
                    fmt_pct(verdict.compare_delta_inst_pct),
                    "`PASS`" if verdict.status == "pass" else "`FAIL`",
                ]
            )
            + " |"
        )
    return lines


def render_point_notes(verdicts: list[Verdict]) -> list[str]:
    lines: list[str] = []
    for verdict in sorted(verdicts, key=lambda item: item.points):
        extra = ""
        if verdict.secondary_sample is not None and verdict.secondary_delta_cycle_pct is not None:
            extra = (
                f" 低一档 r{verdict.secondary_sample.repeat} -> r{verdict.canonical_repeat} 的 cycle 变化是 "
                f"{verdict.secondary_delta_cycle_pct:.3f}%，inst 变化是 {verdict.secondary_delta_inst_pct:.3f}%。"
            )
        lines.append(f"- `N={verdict.points}`: {verdict.note}{extra}")
    return lines


def summarize_impl(verdicts: list[Verdict]) -> list[str]:
    passed = [item for item in verdicts if item.status == "pass"]
    failed = [item for item in verdicts if item.status == "fail"]
    all_failed_small_inst = all(
        item.compare_delta_inst_pct is not None and item.compare_delta_inst_pct < 0.2
        for item in failed
    )

    points_text = "、".join(f"`{item.points}`" for item in passed) if passed else "无"
    fail_text = "、".join(f"`{item.points}`" for item in failed) if failed else "无"

    lines = [
        f"- 通过 1% 收敛判据的点数：{points_text}。",
        f"- 未通过的点数：{fail_text}。",
    ]
    if failed and all_failed_small_inst:
        lines.append(
            "- 未通过点的 `平均指令/次` 变化都很小，而 `平均周期/次` 仍显著下滑，"
            "更像是 warmup 不足导致首轮带着额外 cycle 税进入 measured batch，而不是算法本体工作量还在变。"
        )
    return lines


def render_markdown(run_dir: Path, verdicts: list[Verdict]) -> str:
    scalar = [item for item in verdicts if item.version == "scalar"]
    rvv = [item for item in verdicts if item.version == "rvv"]
    scalar_failed = [item for item in scalar if item.status == "fail"]
    rvv_failed = [item for item in rvv if item.status == "fail"]

    lines: list[str] = [
        "# FFT Steady-State 收敛复核",
        "",
        f"- 数据目录：`{run_dir}`",
        "- 判据：按 `overview/policy/20260331-fft-steady-state-standard.analysis.md`，相邻 warmed 档位的 `平均周期/次` 差异不超过约 `1%` 才算进入可发布 steady state。",
        "- 主口径：全部使用 batch-level `FFT_ROI cycle / instret / IPC`；`whole-program` 只作为辅助信息，没有用于收敛判定。",
        "",
        "## 1. 总结论",
        "",
        f"- Scalar：`{len(scalar) - len(scalar_failed)}/{len(scalar)}` 个点通过；当前 `warmup=2` 下，`16-4096` 没有一个点满足现行 canonical repeat 的 1% 收敛要求。",
        f"- RVV：`{len(rvv) - len(rvv_failed)}/{len(rvv)}` 个点通过；`256/512/1024/2048` 已收敛，`16/64/128` 仍未收敛。",
        "- 因为 same-point A/B 发布要求相同 warmup policy 与相同 measured repeat，而 scalar baseline 现在没有收敛，所以这张 sweep 不能直接拿来确认现行 unified canonical 参数已经成立。",
        "",
        "## 2. Scalar 结果",
        "",
        *summarize_impl(scalar),
        "",
        *render_group_table(scalar),
        "",
        *render_point_notes(scalar),
        "",
        "## 3. RVV 结果",
        "",
        *summarize_impl(rvv),
        "",
        *render_group_table(rvv),
        "",
        *render_point_notes(rvv),
        "",
        "## 4. 对当前 canonical 参数的含义",
        "",
        "- `N=16`：无论 scalar 还是 RVV，`r30` 到更高 repeat 都还在明显下降，现行 `r30` 不能视作稳态已证实。",
        "- `N=64/128`：RVV 已经接近收敛边界，但仍略高于 1%；scalar 仍远未收敛，所以统一 canonical 还不能保留在 `r10`。",
        "- `N=256/512`：RVV 的 `r10` 已足够，但 scalar 的 `r10` 仍明显偏低。统一发布口径如果仍要和 scalar 配平，就不能直接沿用当前 `r10`。",
        "- `N=1024/2048/4096`：RVV 的 `r5/r3` 已足够，但 scalar 在 `r5/r3` 甚至更高 repeat 上仍持续下降。对于大点数，问题显然不只是 repeat 太小，更像是 `warmup=2` 还没有把首轮额外 cycle 税完全推到 ROI 外。",
        "",
        "## 5. 建议的下一步路径",
        "",
        "- 不建议先继续把 scalar 的 measured repeat 无限制往上堆；从这批数据看，`平均指令/次` 基本不动而 `平均周期/次` 持续掉，优先怀疑的是 warmup 不足。",
        "- 更合理的下一轮应该先做 `scalar warmup sweep`，而 measured repeat 先固定在当前各点已测的较高档：`N=16 -> r40`，`N=64/128/256/512 -> r20`，`N=1024/2048/4096 -> r7`。",
        "- warmup sweep 可以先试 `warmup=4/6/8`。如果某点在这些 warmup 下已经把相邻 repeat 差异压进 1%，再回头确定新的 unified canonical repeat。",
        "- 等 scalar 侧 warmup 收敛以后，再只对 `RVV 16/64/128` 做补测；`RVV 256+` 这批数据已经说明 repeat 本身基本够了。",
        "",
        "## 6. 证据文件",
        "",
        f"- 原始矩阵：`{run_dir / 'matrix.tsv'}`",
        f"- 批次状态：`{run_dir / 'status.log'}`",
    ]
    return "\n".join(lines) + "\n"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--run-dir", required=True, help="scripts/run_steady_state_convergence_sweep.sh 产出的 stage root")
    args = parser.parse_args()

    run_dir = Path(args.run_dir).resolve()
    samples = load_samples(run_dir)
    grouped: dict[tuple[str, int], list[Sample]] = {}
    for sample in samples:
        grouped.setdefault((sample.version, sample.points), []).append(sample)

    verdicts = [
        build_verdict(version, points, grouped[(version, points)])
        for version, points in sorted(grouped)
    ]

    tsv_path = run_dir / "steady-state-convergence.tsv"
    md_path = run_dir / "steady-state-convergence.analysis.md"
    tsv_path.write_text(render_tsv(verdicts))
    md_path.write_text(render_markdown(run_dir, verdicts))

    print(f"Wrote {tsv_path}")
    print(f"Wrote {md_path}")


if __name__ == "__main__":
    main()
