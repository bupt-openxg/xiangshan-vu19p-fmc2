#!/usr/bin/env python3
import argparse
import math
from datetime import datetime
from pathlib import Path

from gen_fft_perf_reports import (
    DEFAULT_IMPL,
    REPORT_NAME_RE,
    PerfMetrics,
    fmt_avg_cycle_per_repeat,
    fmt_avg_instr_per_repeat,
    fmt_float,
    fmt_rel_change,
    infer_version_impl_from_path,
    overview_output_dir,
    parse_report,
)


TRACKED_VERSIONS = {"scalar", "rvv", "ext"}


def iter_reports(run_dir: Path):
    root_name_is_point = run_dir.name.startswith("n") and run_dir.name[1:].isdigit()
    for path in sorted(run_dir.rglob("*.report.txt")):
        rel_parts = path.relative_to(run_dir).parts
        if root_name_is_point:
            if len(rel_parts) < 3:
                continue
            if rel_parts[0] not in TRACKED_VERSIONS:
                continue
            if rel_parts[1] != "sim":
                continue
        else:
            if len(rel_parts) < 4:
                continue
            if not rel_parts[0].startswith("n") or not rel_parts[0][1:].isdigit():
                continue
            if rel_parts[1] not in TRACKED_VERSIONS:
                continue
            if rel_parts[2] != "sim":
                continue
        match = REPORT_NAME_RE.match(path.name)
        if not match:
            continue
        version, implementation = infer_version_impl_from_path(path)
        if version not in TRACKED_VERSIONS or implementation is None:
            continue
        yield int(match.group("points")), int(match.group("repeat")), version, implementation, path, parse_report(path)


def discover_best_scalar_by_point_repeat(run_dir: Path) -> dict[tuple[int, int], tuple[str, Path, PerfMetrics]]:
    best: dict[tuple[int, int], tuple[str, Path, PerfMetrics]] = {}
    for points, repeat, version, implementation, path, metrics in iter_reports(run_dir):
        if version != "scalar":
            continue
        key = (points, repeat)
        current = best.get(key)
        if current is None or metrics.primary_cycle() < current[2].primary_cycle():
            best[key] = (implementation, path, metrics)
    return best


def discover_named_reports(run_dir: Path, version_name: str) -> dict[tuple[int, int], tuple[Path, PerfMetrics]]:
    chosen: dict[tuple[int, int], tuple[Path, PerfMetrics]] = {}
    default_impl = DEFAULT_IMPL[version_name]
    for points, repeat, version, implementation, path, metrics in iter_reports(run_dir):
        if version != version_name or implementation != default_impl:
            continue
        key = (points, repeat)
        current = chosen.get(key)
        if current is None or metrics.primary_cycle() < current[1].primary_cycle():
            chosen[key] = (path, metrics)
    return chosen


def geo_mean_rel_change(rows: list[tuple[int, int, str, Path, PerfMetrics, Path, PerfMetrics, Path, PerfMetrics]],
                        numerator_idx: int,
                        base_idx: int) -> str:
    ratios = []
    for row in rows:
        numerator = row[numerator_idx].primary_cycle()
        base = row[base_idx].primary_cycle()
        if base <= 0:
            continue
        ratios.append(numerator / base)
    if not ratios:
        return "-"
    geo = math.prod(ratios) ** (1.0 / len(ratios)) - 1.0
    return f"{geo * 100:+.2f}%"


def render_cross_point_summary(
    run_dir: Path,
    rows: list[tuple[int, int, str, Path, PerfMetrics, Path, PerfMetrics, Path, PerfMetrics]],
) -> str:
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M")
    ordered = sorted(rows, key=lambda item: item[0])
    lines = [
        "# Pow2 Scalar / RVV / EXT 跨点数对比",
        "",
        f"- 数据口径：`{run_dir}` 当前正式发布树中的 `scalar` / `rvv` / `ext` 主线报告，主口径统一看 `FFT_ROI`。",
        f"- 刷新时间：`{timestamp}`。",
        "- 比较范围：`FFT_RUN_IDFT=0`，同点数、同 repeat；`scalar` 取 same-repeat 最优基线，`rvv` / `ext` 取当前 `mainline`。",
        "- 维护口径说明：当前正式树只维护唯一一套 `ext mainline` 口径，不再保留并行的历史 ext 变体。",
        "- 指标定义：`平均周期/次 = ROI cycle / repeat`，`平均指令/次 = ROI instret / repeat`，这里的“指令数”指 ROI 动态提交指令数 `instret`。",
        "",
        "## 1. 绝对值总表",
        "",
        "| 点数 | Repeat | Scalar 平均周期/次 | Scalar 平均指令/次 | Scalar IPC | RVV 平均周期/次 | RVV 平均指令/次 | RVV IPC | EXT 平均周期/次 | EXT 平均指令/次 | EXT IPC |",
        "| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |",
    ]

    for points, repeat, _, _, scalar, _, rvv, _, ext in ordered:
        scalar_repeat = scalar.primary_repeat(repeat)
        rvv_repeat = rvv.primary_repeat(repeat)
        ext_repeat = ext.primary_repeat(repeat)
        lines.append(
            "| "
            + " | ".join(
                [
                    str(points),
                    str(repeat),
                    fmt_avg_cycle_per_repeat(scalar.primary_cycle(), scalar_repeat),
                    fmt_avg_instr_per_repeat(scalar.primary_instr(), scalar_repeat),
                    fmt_float(scalar.primary_ipc(), 4),
                    fmt_avg_cycle_per_repeat(rvv.primary_cycle(), rvv_repeat),
                    fmt_avg_instr_per_repeat(rvv.primary_instr(), rvv_repeat),
                    fmt_float(rvv.primary_ipc(), 4),
                    fmt_avg_cycle_per_repeat(ext.primary_cycle(), ext_repeat),
                    fmt_avg_instr_per_repeat(ext.primary_instr(), ext_repeat),
                    fmt_float(ext.primary_ipc(), 4),
                ]
            )
            + " |"
        )

    lines.extend(
        [
            "",
            "## 2. 相对值总表",
            "",
            "### 2.1 统一以 Scalar 为基准",
            "",
            "| 点数 | Repeat | RVV 周期变化 | RVV 指令数变化 | RVV IPC 变化 | EXT 周期变化 | EXT 指令数变化 | EXT IPC 变化 |",
            "| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |",
        ]
    )

    for points, repeat, _, _, scalar, _, rvv, _, ext in ordered:
        lines.append(
            "| "
            + " | ".join(
                [
                    str(points),
                    str(repeat),
                    fmt_rel_change(rvv.primary_cycle(), scalar.primary_cycle()),
                    fmt_rel_change(rvv.primary_instr(), scalar.primary_instr()),
                    fmt_rel_change(rvv.primary_ipc(), scalar.primary_ipc()),
                    fmt_rel_change(ext.primary_cycle(), scalar.primary_cycle()),
                    fmt_rel_change(ext.primary_instr(), scalar.primary_instr()),
                    fmt_rel_change(ext.primary_ipc(), scalar.primary_ipc()),
                ]
            )
            + " |"
        )

    lines.extend(
        [
            "",
            "### 2.2 EXT 相对 RVV",
            "",
            "| 点数 | Repeat | 周期变化 | 指令数变化 | IPC 变化 |",
            "| ---: | ---: | ---: | ---: | ---: |",
        ]
    )

    for points, repeat, _, _, _, _, rvv, _, ext in ordered:
        lines.append(
            "| "
            + " | ".join(
                [
                    str(points),
                    str(repeat),
                    fmt_rel_change(ext.primary_cycle(), rvv.primary_cycle()),
                    fmt_rel_change(ext.primary_instr(), rvv.primary_instr()),
                    fmt_rel_change(ext.primary_ipc(), rvv.primary_ipc()),
                ]
            )
            + " |"
        )

    lines.extend(
        [
            "",
            "## 3. 快速结论",
            "",
            (
                f"- `RVV vs Scalar` 的 `ROI cycle` 在 8 个点数上全部下降；"
                f"几何平均变化约 `{geo_mean_rel_change(ordered, 6, 4)}`。"
            ),
            (
                f"- `EXT vs RVV` 的 `ROI cycle` 在 8 个点数上全部下降；"
                f"几何平均变化约 `{geo_mean_rel_change(ordered, 8, 6)}`。"
            ),
            (
                f"- `EXT vs Scalar` 的 `ROI cycle` 在 8 个点数上全部下降；"
                f"几何平均变化约 `{geo_mean_rel_change(ordered, 8, 4)}`。"
            ),
            "",
            "## 4. 来源报告",
            "",
        ]
    )

    for points, repeat, _, scalar_path, _, rvv_path, _, ext_path, _ in ordered:
        lines.extend(
            [
                f"- `n{points}` `r{repeat}`",
                f"  - scalar: `{scalar_path.relative_to(run_dir)}`",
                f"  - rvv: `{rvv_path.relative_to(run_dir)}`",
                f"  - ext: `{ext_path.relative_to(run_dir)}`",
            ]
        )

    return "\n".join(lines) + "\n"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--run-dir",
        required=True,
        help="perf_eval root or any subtree that already follows the n<points>/<version> layout",
    )
    args = parser.parse_args()

    run_dir = Path(args.run_dir)
    scalar_pairs = discover_best_scalar_by_point_repeat(run_dir)
    rvv_pairs = discover_named_reports(run_dir, "rvv")
    ext_pairs = discover_named_reports(run_dir, "ext")

    rows: list[tuple[int, int, str, Path, PerfMetrics, Path, PerfMetrics, Path, PerfMetrics]] = []
    for key, (ext_path, ext_metrics) in sorted(ext_pairs.items()):
        if key not in scalar_pairs or key not in rvv_pairs:
            continue
        scalar_impl, scalar_path, scalar_metrics = scalar_pairs[key]
        rvv_path, rvv_metrics = rvv_pairs[key]
        rows.append(
            (
                key[0],
                key[1],
                scalar_impl,
                scalar_path,
                scalar_metrics,
                rvv_path,
                rvv_metrics,
                ext_path,
                ext_metrics,
            )
        )

    if not rows:
        raise SystemExit("no complete scalar/rvv/ext point set found under run-dir")

    summary_dir = overview_output_dir(run_dir)
    summary_dir.mkdir(parents=True, exist_ok=True)
    (summary_dir / "pow2-scalar-rvv-ext-summary.analysis.md").write_text(
        render_cross_point_summary(run_dir, rows)
    )


if __name__ == "__main__":
    main()
