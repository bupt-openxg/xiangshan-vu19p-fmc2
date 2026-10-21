#!/usr/bin/env python3
import argparse
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path

from gen_fft_perf_reports import REPORT_NAME_RE, parse_report


WHOLE_COUNTER_KEYS = [
    "stallCycles_fetch_icacheMain",
    "stallCycles_decodeFull",
    "stallCycles_ibufferFull",
    "waitLoadCycle",
    "waitStoreCycle",
    "commit_branch_num",
    "commit_branch_mispredicts",
]


@dataclass
class Sample:
    version: str
    points: int
    repeat: int
    warmup: int
    roi_cycle: int
    roi_instret: int
    avg_cycle: float
    avg_instret: float
    roi_ipc: float
    roi_hpm: dict[tuple[str, str], int]
    whole: dict[str, int]
    report_path: Path


def fmt_float(value: float, digits: int = 2) -> str:
    return f"{value:,.{digits}f}"


def fmt_pct(value: float) -> str:
    return f"{value:+.3f}%"


def pct_delta(base: float, other: float) -> float:
    if base == 0:
        return 0.0
    return (other - base) / base * 100.0


def parse_warmup(path: Path) -> int:
    for line in path.read_text(errors="replace").splitlines():
        if line.startswith("FFT_STEADY_STATE "):
            parts = dict(item.split("=") for item in line.split()[1:])
            return int(parts.get("warmup", "0"))
    return 0


def load_samples(run_dir: Path) -> dict[tuple[str, int], list[Sample]]:
    grouped: dict[tuple[str, int], list[Sample]] = defaultdict(list)
    for path in sorted(run_dir.glob("n*/**/sim/*/*.report.txt")):
        match = REPORT_NAME_RE.match(path.name)
        if not match:
            continue
        perf = parse_report(path)
        if perf.roi is None:
            continue
        version = path.parts[-4]
        points = int(match.group("points"))
        repeat = int(match.group("repeat"))
        grouped[(version, points)].append(
            Sample(
                version=version,
                points=points,
                repeat=repeat,
                warmup=parse_warmup(path),
                roi_cycle=perf.roi.cycle,
                roi_instret=perf.roi.instret,
                avg_cycle=perf.roi.cycle / perf.roi.repeat,
                avg_instret=perf.roi.instret / perf.roi.repeat,
                roi_ipc=perf.roi.instret / perf.roi.cycle if perf.roi.cycle else 0.0,
                roi_hpm=perf.roi.hpm,
                whole={key: perf.get(key) for key in WHOLE_COUNTER_KEYS},
                report_path=path,
            )
        )
    return grouped


def render_pair_section(version: str, points: int, low: Sample, high: Sample) -> list[str]:
    hpm_keys = sorted(set(low.roi_hpm) | set(high.roi_hpm))
    lines = [
        f"### {version.upper()} N={points}",
        "",
        f"- 比较档位：`r{low.repeat}` vs `r{high.repeat}`，warmup 都是 `{low.warmup}`。",
        f"- ROI `平均周期/次`：`{fmt_float(low.avg_cycle)}` -> `{fmt_float(high.avg_cycle)}` (`{fmt_pct(pct_delta(low.avg_cycle, high.avg_cycle))}`)",
        f"- ROI `平均指令/次`：`{fmt_float(low.avg_instret)}` -> `{fmt_float(high.avg_instret)}` (`{fmt_pct(pct_delta(low.avg_instret, high.avg_instret))}`)",
        f"- ROI IPC：`{low.roi_ipc:.4f}` -> `{high.roi_ipc:.4f}`",
        "",
        "| ROI HPM 事件 | r低 per FFT | r高 per FFT | 变化 |",
        "| --- | ---: | ---: | ---: |",
    ]
    for key in hpm_keys:
        low_v = low.roi_hpm.get(key, 0) / low.repeat
        high_v = high.roi_hpm.get(key, 0) / high.repeat
        lines.append(
            f"| `{key[0]}.{key[1]}` | {fmt_float(low_v)} | {fmt_float(high_v)} | {fmt_pct(pct_delta(low_v, high_v))} |"
        )

    lines.extend(
        [
            "",
            "- 下表是 whole-program 辅助计数器，仍按 `每次 FFT = total / measured_repeat` 归一化；"
            "它们不等于 ROI 纯核成本，但能帮助判断 warmup 相关的固定税更偏前端还是等待类。",
            "",
            "| Whole-program 辅助事件 | r低 per FFT | r高 per FFT | 变化 |",
            "| --- | ---: | ---: | ---: |",
        ]
    )
    for key in WHOLE_COUNTER_KEYS:
        low_v = low.whole.get(key, 0) / low.repeat
        high_v = high.whole.get(key, 0) / high.repeat
        lines.append(f"| `{key}` | {fmt_float(low_v)} | {fmt_float(high_v)} | {fmt_pct(pct_delta(low_v, high_v))} |")

    if low.whole.get("commit_branch_num", 0) and high.whole.get("commit_branch_num", 0):
        low_rate = low.whole["commit_branch_mispredicts"] / low.whole["commit_branch_num"] * 100.0
        high_rate = high.whole["commit_branch_mispredicts"] / high.whole["commit_branch_num"] * 100.0
        lines.extend(
            [
                "",
                f"- whole-program 分支失预测率：`{low_rate:.3f}%` -> `{high_rate:.3f}%`",
            ]
        )

    lines.extend(
        [
            "",
            f"- 低 repeat 报告：`{low.report_path}`",
            f"- 高 repeat 报告：`{high.report_path}`",
            "",
        ]
    )
    return lines


def render_summary(grouped: dict[tuple[str, int], list[Sample]]) -> str:
    sections = [
        "# FFT 波动诊断",
        "",
        "- 本页聚焦上一轮波动最大的代表点，只看同 warmup 下的低 repeat / 高 repeat 对比。",
        "- 主判断仍以 ROI `平均周期/次` 与 ROI HPM 为主；whole-program 计数器只做辅助归因。",
        "",
    ]

    for version, points in sorted(grouped):
        samples = sorted(grouped[(version, points)], key=lambda item: item.repeat)
        if len(samples) < 2:
            continue
        sections.extend(render_pair_section(version, points, samples[0], samples[-1]))
    return "\n".join(sections) + "\n"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--run-dir", required=True)
    parser.add_argument(
        "--points",
        default="",
        help="comma-separated point list; empty means all available points",
    )
    parser.add_argument(
        "--output",
        default="",
        help="optional output markdown path; default is <run-dir>/volatility-diagnostic.analysis.md",
    )
    args = parser.parse_args()

    run_dir = Path(args.run_dir).resolve()
    grouped = load_samples(run_dir)
    selected_points = None
    if args.points:
        selected_points = {int(item) for item in args.points.split(",") if item.strip()}
        grouped = {
            key: value
            for key, value in grouped.items()
            if key[1] in selected_points
        }

    output_path = Path(args.output).resolve() if args.output else (run_dir / "volatility-diagnostic.analysis.md")
    output_path.write_text(render_summary(grouped))
    print(f"Wrote {output_path}")


if __name__ == "__main__":
    main()
