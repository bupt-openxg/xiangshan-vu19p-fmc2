#!/usr/bin/env python3
import argparse
from datetime import datetime
from pathlib import Path

from gen_fft_perf_reports import (
    DEFAULT_IMPL,
    REPORT_NAME_RE,
    PerfMetrics,
    fmt_avg_cycle_per_repeat,
    fmt_avg_instr_per_repeat,
    fmt_float,
    fmt_int,
    fmt_pct,
    fmt_improvement,
    fmt_rel_change,
    frontend_breakdown_text,
    infer_version_impl_from_path,
    parse_report,
    render_key_counter_table,
    render_term_glossary,
    overview_output_dir,
)


def scalar_display_name(implementation: str) -> str:
    return f"标量基线（`{implementation}`）"


def rvv_display_name(implementation: str) -> str:
    if implementation == "mainline":
        return "RVV 主线"
    return f"RVV `{implementation}`"


def iter_reports(run_dir: Path):
    root_name_is_point = run_dir.name.startswith("n") and run_dir.name[1:].isdigit()
    for path in sorted(run_dir.rglob("*.report.txt")):
        rel_parts = path.relative_to(run_dir).parts
        if root_name_is_point:
            if len(rel_parts) < 3:
                continue
            if rel_parts[0] not in {"oai", "scalar", "rvv"}:
                continue
            if rel_parts[1] != "sim":
                continue
        else:
            if len(rel_parts) < 4:
                continue
            if not rel_parts[0].startswith("n") or not rel_parts[0][1:].isdigit():
                continue
            if rel_parts[1] not in {"oai", "scalar", "rvv"}:
                continue
            if rel_parts[2] != "sim":
                continue
        match = REPORT_NAME_RE.match(path.name)
        if not match:
            continue
        version, implementation = infer_version_impl_from_path(path)
        if version is None or implementation is None:
            continue
        points = int(match.group("points"))
        repeat = int(match.group("repeat"))
        yield points, repeat, version, implementation, path, parse_report(path)


def discover_best_scalar_by_point_repeat(run_dir: Path) -> dict[tuple[int, int], tuple[str, PerfMetrics]]:
    best: dict[tuple[int, int], tuple[str, PerfMetrics]] = {}
    for points, repeat, version, implementation, _, metrics in iter_reports(run_dir):
        if version != "scalar":
            continue
        key = (points, repeat)
        current = best.get(key)
        if current is None or metrics.primary_cycle() < current[1].primary_cycle():
            best[key] = (implementation, metrics)
    return best


def discover_rvv_reports(run_dir: Path) -> list[tuple[int, int, str, Path, PerfMetrics]]:
    reports: list[tuple[int, int, str, Path, PerfMetrics]] = []
    for points, repeat, version, implementation, path, metrics in iter_reports(run_dir):
        if version != "rvv":
            continue
        reports.append((points, repeat, implementation, path, metrics))
    return reports


def render_pair_overview(
    repeat: int,
    scalar_impl: str,
    scalar: PerfMetrics,
    rvv_impl: str,
    rvv: PerfMetrics,
) -> str:
    scalar_repeat = scalar.primary_repeat(repeat)
    rvv_repeat = rvv.primary_repeat(repeat)
    return "\n".join(
        [
            "| 实现 | 主口径 | ROI instret | ROI cycle | 平均指令/次 | 平均周期/次 | ROI IPC | ROI CPI | whole instrCnt | whole cycleCnt |",
            "| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |",
            "| "
            + " | ".join(
                [
                    scalar_display_name(scalar_impl),
                    scalar.scope_label(),
                    fmt_int(scalar.primary_instr()),
                    fmt_int(scalar.primary_cycle()),
                    fmt_avg_instr_per_repeat(scalar.primary_instr(), scalar_repeat),
                    fmt_avg_cycle_per_repeat(scalar.primary_cycle(), scalar_repeat),
                    fmt_float(scalar.primary_ipc(), 6),
                    fmt_float(scalar.primary_cpi(), 6),
                    fmt_int(scalar.instr),
                    fmt_int(scalar.cycle),
                ]
            )
            + " |",
            "| "
            + " | ".join(
                [
                    rvv_display_name(rvv_impl),
                    rvv.scope_label(),
                    fmt_int(rvv.primary_instr()),
                    fmt_int(rvv.primary_cycle()),
                    fmt_avg_instr_per_repeat(rvv.primary_instr(), rvv_repeat),
                    fmt_avg_cycle_per_repeat(rvv.primary_cycle(), rvv_repeat),
                    fmt_float(rvv.primary_ipc(), 6),
                    fmt_float(rvv.primary_cpi(), 6),
                    fmt_int(rvv.instr),
                    fmt_int(rvv.cycle),
                ]
            )
            + " |",
        ]
    )


def render_roi_hpm_pair_table(repeat: int, scalar: PerfMetrics, rvv: PerfMetrics) -> str:
    scalar_repeat = scalar.primary_repeat(repeat)
    rvv_repeat = rvv.primary_repeat(repeat)
    scalar_hpm = dict(scalar.roi_hpm_items())
    rvv_hpm = dict(rvv.roi_hpm_items())
    all_events = sorted(set(scalar_hpm) | set(rvv_hpm))

    lines = [
        "| 分组 | 事件 | Scalar ROI 总量 | Scalar 平均/次 | RVV ROI 总量 | RVV 平均/次 | RVV 相对 Scalar 变化 |",
        "| --- | --- | ---: | ---: | ---: | ---: | ---: |",
    ]

    for group, event in all_events:
        scalar_value = scalar_hpm.get((group, event), 0)
        rvv_value = rvv_hpm.get((group, event), 0)
        lines.append(
            "| "
            + " | ".join(
                [
                    group,
                    f"`{event}`",
                    fmt_int(scalar_value),
                    fmt_avg_cycle_per_repeat(scalar_value, scalar_repeat),
                    fmt_int(rvv_value),
                    fmt_avg_cycle_per_repeat(rvv_value, rvv_repeat),
                    fmt_rel_change(rvv_value, scalar_value),
                ]
            )
            + " |"
        )
    return "\n".join(lines)


def render_rvv_report(
    points: int,
    repeat: int,
    implementation: str,
    rvv_path: Path,
    rvv: PerfMetrics,
    scalar_impl: str,
    scalar: PerfMetrics,
) -> str:
    rvv_name = rvv_display_name(implementation)
    scalar_name = scalar_display_name(scalar_impl)
    roi_note = (
        "- 主表优先使用 batch-level `FFT_ROI` 的 `instret/cycle/IPC`；`emu` 结尾的 whole-program summary 只作为辅助列。"
        if rvv.has_roi() and scalar.has_roi()
        else "- 当前配对里仍有旧 report 缺少 `FFT_ROI`，主表会对缺失侧回退到 whole-program summary。"
    )

    lines = [
        f"# RVV DFT{points} 性能分析",
        "",
        f"- 报告文件：`{rvv_path.name}`、`{scalar.path.name}`",
        f"- 工作负载：`FFT_POINTS={points} FFT_REPEAT={repeat} FFT_RUN_IDFT=0 FFT_VALIDATE=0`",
        f"- 对比口径：当前页固定比较 {rvv_name} 与 same-repeat 的 {scalar_name}。",
        "- 这里的 same-repeat scalar 只用于这一页的 RVV 对照，不把不同 repeat 的 whole-program 总量混在一起解释。",
        roi_note,
        "",
        render_term_glossary(points, include_helper=False),
        "",
        "## 1. 总体结果",
        "",
        render_pair_overview(repeat, scalar_impl, scalar, implementation, rvv),
        "",
        (
            f"- 主口径下，{rvv_name} 相对 {scalar_name} 的 `ROI cycle` 变化是 "
            f"`{fmt_rel_change(rvv.primary_cycle(), scalar.primary_cycle())}`，"
            f"也就是周期下降了 `{fmt_improvement(rvv.primary_cycle(), scalar.primary_cycle())}`。"
        ),
        (
            f"- `ROI instret` 相对变化是 `{fmt_rel_change(rvv.primary_instr(), scalar.primary_instr())}`；"
            f"`ROI IPC` 分别是 `{fmt_float(rvv.primary_ipc(), 6)}` 和 `{fmt_float(scalar.primary_ipc(), 6)}`。"
        ),
        "",
    ]

    if rvv.roi_hpm_items() and scalar.roi_hpm_items():
        lines.extend(
            [
                "## 2. ROI HPM 关键事件",
                "",
                "- 这里只保留当前 build 上 selector 能确认、且对 FFT attribution 最关键的事件；不支持的事件不写空列。",
                "",
                render_roi_hpm_pair_table(repeat, scalar, rvv),
                "",
            ]
        )

    lines.extend(
        [
            "## 3. Whole-program 关键计数器总览",
            "",
            render_key_counter_table("RVV", rvv, "Scalar", scalar, "left_right"),
            "",
            "## 4. 结果解读",
            "",
            "### 4.1 ROI 主口径先回答“值不值得”",
            "",
            (
                f"- 这一页的优先结论只看 same-repeat `ROI cycle`。"
                f"当前 {rvv_name} 平均周期/次是 `{fmt_avg_cycle_per_repeat(rvv.primary_cycle(), rvv.primary_repeat(repeat))}`，"
                f"{scalar_name} 是 `{fmt_avg_cycle_per_repeat(scalar.primary_cycle(), scalar.primary_repeat(repeat))}`。"
            ),
            (
                f"- 如果 `ROI IPC` 低于 scalar，这里也不直接判负面；更关键的是"
                f" RVV 是否已经把 `ROI instret` 和最终 `ROI cycle` 一起压下来。"
            ),
            "",
            "### 4.2 ROI HPM 用来回答“慢在哪里”",
            "",
        ]
    )

    if rvv.roi_hpm_items() and scalar.roi_hpm_items():
        scalar_hpm = dict(scalar.roi_hpm_items())
        rvv_hpm = dict(rvv.roi_hpm_items())
        lines.extend(
            [
                (
                    f"- `decoder_stall_cycle` 平均/次：scalar `{fmt_avg_cycle_per_repeat(scalar_hpm.get(('backend', 'decoder_stall_cycle'), 0), scalar.primary_repeat(repeat))}`，"
                    f"RVV `{fmt_avg_cycle_per_repeat(rvv_hpm.get(('backend', 'decoder_stall_cycle'), 0), rvv.primary_repeat(repeat))}`。"
                ),
                (
                    f"- `rob_commitInstrLoad/Store` 平均/次：scalar "
                    f"`{fmt_avg_cycle_per_repeat(scalar_hpm.get(('backend', 'rob_commitInstrLoad'), 0), scalar.primary_repeat(repeat))}` / "
                    f"`{fmt_avg_cycle_per_repeat(scalar_hpm.get(('backend', 'rob_commitInstrStore'), 0), scalar.primary_repeat(repeat))}`，"
                    f"RVV "
                    f"`{fmt_avg_cycle_per_repeat(rvv_hpm.get(('backend', 'rob_commitInstrLoad'), 0), rvv.primary_repeat(repeat))}` / "
                    f"`{fmt_avg_cycle_per_repeat(rvv_hpm.get(('backend', 'rob_commitInstrStore'), 0), rvv.primary_repeat(repeat))}`。"
                ),
                (
                    f"- `ifu_miss` 在这套小而确定的 ROI 窗口里通常更像固定前端冷启动信号；"
                    f"如果它几乎不随 repeat 增长，解读时要把它当辅助噪声而不是主瓶颈。"
                ),
            ]
        )
    else:
        lines.append("- 这组数据没有完整的 ROI HPM 配对，瓶颈解释主要回退到 whole-program 辅助计数器。")

    lines.extend(
        [
            "",
            "### 4.3 Whole-program 计数器只做辅助，不再拿来充当主口径",
            "",
            (
                f"- 当前 whole-program 的前端占比：RVV 是 `{frontend_breakdown_text(rvv)}`，"
                f"scalar 是 `{frontend_breakdown_text(scalar)}`。"
            ),
            (
                f"- `waitLoad/Store` 占 cycle：RVV 是 "
                f"`{fmt_pct(rvv.share_cycle('waitLoadCycle') + rvv.share_cycle('waitStoreCycle'))}`，"
                f"scalar 是 `{fmt_pct(scalar.share_cycle('waitLoadCycle') + scalar.share_cycle('waitStoreCycle'))}`。"
            ),
            (
                "- 这些 whole-program 计数器仍然有参考价值，但它们会混进 ROI 外的输入装载、配置和收尾；"
                "正式结论必须以前面的 ROI 主表为准。"
            ),
            "",
        ]
    )

    return "\n".join(lines)


def analysis_filename(points: int, repeat: int, implementation: str) -> str:
    if implementation == "mainline":
        return f"n{points}-r{repeat}-idft0-rvv.analysis.md"
    return f"n{points}-r{repeat}-idft0-rvv-{implementation}.analysis.md"


def render_cross_point_summary(
    run_dir: Path,
    rows: list[tuple[int, int, str, PerfMetrics, PerfMetrics]],
) -> str:
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M")
    ordered = sorted(rows, key=lambda item: item[0])
    lines = [
        "# Pow2 RVV 主线 vs Scalar 基线跨点数对比",
        "",
        f"- 数据口径：`{run_dir}` 当前正式发布树中的 RVV `mainline` 对比页。",
        f"- 刷新时间：`{timestamp}`。",
        "- 比较范围：`FFT_RUN_IDFT=0`，同点数、同 repeat、same-repeat scalar 基线，主口径统一看 `FFT_ROI`。",
        "- 本页只整理跨点数总表；更细的瓶颈归因仍回到各点数单页分析。",
        "",
        "## 1. 最新对比总表",
        "",
        "| 点数 | Repeat | Scalar 平均指令/次 | RVV 平均指令/次 | Scalar 平均周期/次 | RVV 平均周期/次 | RVV 相对 Scalar 指令变化 | RVV 相对 Scalar 周期变化 |",
        "| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |",
    ]

    for points, repeat, scalar_impl, scalar, rvv in ordered:
        scalar_repeat = scalar.primary_repeat(repeat)
        rvv_repeat = rvv.primary_repeat(repeat)
        lines.append(
            "| "
            + " | ".join(
                [
                    str(points),
                    str(repeat),
                    fmt_avg_instr_per_repeat(scalar.primary_instr(), scalar_repeat),
                    fmt_avg_instr_per_repeat(rvv.primary_instr(), rvv_repeat),
                    fmt_avg_cycle_per_repeat(scalar.primary_cycle(), scalar_repeat),
                    fmt_avg_cycle_per_repeat(rvv.primary_cycle(), rvv_repeat),
                    fmt_rel_change(rvv.primary_instr(), scalar.primary_instr()),
                    fmt_rel_change(rvv.primary_cycle(), scalar.primary_cycle()),
                ]
            )
            + " |"
        )

    lines.extend(
        [
            "",
            "## 2. 快速结论",
            "",
            "- 主判据统一看 `平均周期/次` 和 `RVV 相对 Scalar 周期变化`；`whole-program` 总量只适合回各单页做辅助解释。",
            "- 这张总表只收录当前正式树里的 RVV `mainline`；实验分支和历史试验稿不混进正式跨点数结论。",
            "",
            "## 3. 来源页",
            "",
        ]
    )

    for points, repeat, _, _, _ in ordered:
        lines.append(
            f"- `n{points}`: `perf_eval/n{points}/rvv/analysis/{analysis_filename(points, repeat, DEFAULT_IMPL['rvv'])}`"
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
    mainline_rows: list[tuple[int, int, str, PerfMetrics, PerfMetrics]] = []

    for points, repeat, implementation, path, rvv in discover_rvv_reports(run_dir):
        pair_key = (points, repeat)
        if pair_key not in scalar_pairs:
            continue
        scalar_impl, scalar = scalar_pairs[pair_key]
        analysis_dir = run_dir / f"n{points}" / "rvv" / "analysis"
        if implementation != DEFAULT_IMPL["rvv"]:
            analysis_dir = analysis_dir / implementation
        analysis_dir.mkdir(parents=True, exist_ok=True)
        (analysis_dir / analysis_filename(points, repeat, implementation)).write_text(
            render_rvv_report(points, repeat, implementation, path, rvv, scalar_impl, scalar)
        )
        if implementation == DEFAULT_IMPL["rvv"]:
            mainline_rows.append((points, repeat, scalar_impl, scalar, rvv))

    if mainline_rows:
        summary_dir = overview_output_dir(run_dir)
        summary_dir.mkdir(parents=True, exist_ok=True)
        (summary_dir / "pow2-rvv-vs-scalar-summary.analysis.md").write_text(
            render_cross_point_summary(run_dir, mainline_rows)
        )


if __name__ == "__main__":
    main()
