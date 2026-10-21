#!/usr/bin/env python3
import argparse
import math
import re
from dataclasses import dataclass
from pathlib import Path


ANSI_RE = re.compile(r"\x1b\[[0-9;]*m")
PERF_RE = re.compile(r":\s*([^,]+),\s+([0-9]+)")
SUMMARY_RE = re.compile(
    r"Core-0 instrCnt = ([0-9,]+), cycleCnt = ([0-9,]+), IPC = ([0-9.]+)"
)
ROI_SUMMARY_RE = re.compile(r"FFT_ROI cycle=([0-9]+) instret=([0-9]+) repeat=([0-9]+)")
ROI_HPM_RE = re.compile(r"FFT_ROI_HPM group=([A-Za-z0-9_-]+) event=([A-Za-z0-9_.-]+) value=([0-9]+)")
REPORT_NAME_RE = re.compile(
    r"n(?P<points>[0-9]+)-r(?P<repeat>[0-9]+)-idft0-(?P<label>[a-z0-9-]+)\.report\.txt"
)

COUNTER_LABELS = [
    "instrCnt",
    "cycleCnt",
    "IPC",
    "CPI",
    "stallCycles_fetch_icacheMain",
    "stallCycles_decodeFull",
    "stallCycles_ibufferFull",
    "commitInstrLoad",
    "commitInstrStore",
    "waitLoadCycle",
    "waitStoreCycle",
    "squashCycles_memVio",
    "commit_branch_mispredicts / commit_branch_num",
]

SCALAR_LEAF_REFS = [
    ("16 点标量蝶形主体", "dft_scalar.c:151"),
    ("4x4 原地转置", "dft_scalar.c:138"),
    ("Q15 复乘与收窄", "dft_scalar.c:111"),
]

OAI_LEAF_REFS = [
    ("packed shuffle / sign 语义链", "oai_dfts.c:54"),
    ("packed_cmult2() + cpack()", "oai_dfts.c:60"),
    ("16 点 OAI minimal leaf", "oai_dfts.c:77"),
]

SHARED_REFS = [
    ("split_radix2/4 输入重排", "include/fft_pow2_scalar_impl.h:248"),
    ("radix2/4 combine + twiddle 复乘", "include/fft_pow2_scalar_impl.h:272"),
    ("多点数层级拼装", "include/fft_pow2_scalar_impl.h:350"),
]

DEFAULT_IMPL = {
    "oai": "minimal",
    "scalar": "mainline",
    "rvv": "mainline",
    "ext": "mainline",
}


def path_is_point_dir(path: Path) -> bool:
    return path.name.startswith("n") and path.name[1:].isdigit()


def overview_output_dir(run_dir: Path) -> Path:
    if path_is_point_dir(run_dir):
        return run_dir
    return run_dir / "overview" / "reports"


def scalar_summary_output_dir(run_dir: Path) -> Path:
    if path_is_point_dir(run_dir):
        return run_dir
    return run_dir / "overview" / "policy"

COMPARISON_REPORT_TEMPLATES = [
    "n{points}-r{repeat}-idft0-oai-vs-scalar.analysis.md",
    "n{points}-r{repeat}-idft0-comparison.analysis.md",
]

SUPPORTED_VERSIONS = {"oai", "scalar", "ext"}

SCALAR_CANONICAL_REPEAT = {
    16: 30,
    64: 10,
    128: 10,
    256: 10,
    512: 10,
    1024: 5,
    2048: 3,
    4096: 3,
}


@dataclass
class PerfMetrics:
    path: Path
    counters: dict
    instr: int
    cycle: int
    ipc: float
    roi: "RoiMetrics | None" = None

    def has_roi(self) -> bool:
        return self.roi is not None

    def scope_label(self) -> str:
        return "ROI" if self.has_roi() else "whole-program"

    def primary_instr(self) -> int:
        if self.roi is not None:
            return self.roi.instret
        return self.instr

    def primary_cycle(self) -> int:
        if self.roi is not None:
            return self.roi.cycle
        return self.cycle

    def primary_ipc(self) -> float:
        cycle = self.primary_cycle()
        if cycle == 0:
            return 0.0
        return self.primary_instr() / cycle

    def primary_cpi(self) -> float:
        instr = self.primary_instr()
        if instr == 0:
            return 0.0
        return self.primary_cycle() / instr

    def primary_repeat(self, fallback_repeat: int) -> int:
        if self.roi is not None and self.roi.repeat > 0:
            return self.roi.repeat
        return fallback_repeat

    def roi_hpm_items(self) -> list[tuple[tuple[str, str], int]]:
        if self.roi is None:
            return []
        return sorted(self.roi.hpm.items(), key=lambda item: item[0])

    @property
    def cpi(self) -> float:
        if self.instr == 0:
            return 0.0
        return self.cycle / self.instr

    def get(self, key: str) -> int:
        return int(self.counters.get(key, 0))

    def share_cycle(self, key: str) -> float:
        if self.cycle == 0:
            return 0.0
        return self.get(key) / self.cycle

    def share_instr(self, key: str) -> float:
        if self.instr == 0:
            return 0.0
        return self.get(key) / self.instr

    def branch_ratio(self) -> float:
        num = self.get("commit_branch_num")
        if num == 0:
            return 0.0
        return self.get("commit_branch_mispredicts") / num


@dataclass
class ReportSelection:
    repeat: int
    scalar_impl: str
    scalar: PerfMetrics
    oai_impl: str
    oai: PerfMetrics


@dataclass
class ScalarSelection:
    repeat: int
    scalar_impl: str
    scalar: PerfMetrics


@dataclass
class RoiMetrics:
    cycle: int
    instret: int
    repeat: int
    hpm: dict[tuple[str, str], int]


def parse_report(path: Path) -> PerfMetrics:
    text = ANSI_RE.sub("", path.read_text())
    counters = {}
    instr = cycle = 0
    ipc = 0.0
    roi_cycle = roi_instret = roi_repeat = None
    roi_hpm: dict[tuple[str, str], int] = {}

    for line in text.splitlines():
        m = PERF_RE.search(line)
        if m:
            counters[m.group(1).strip()] = int(m.group(2))
        m = SUMMARY_RE.search(line)
        if m:
            instr = int(m.group(1).replace(",", ""))
            cycle = int(m.group(2).replace(",", ""))
            ipc = float(m.group(3))
        m = ROI_SUMMARY_RE.search(line)
        if m:
            roi_cycle = int(m.group(1))
            roi_instret = int(m.group(2))
            roi_repeat = int(m.group(3))
        m = ROI_HPM_RE.search(line)
        if m:
            roi_hpm[(m.group(1), m.group(2))] = int(m.group(3))

    if instr == 0 or cycle == 0:
        raise ValueError(f"failed to parse perf summary from {path}")

    roi = None
    if roi_cycle is not None and roi_instret is not None and roi_repeat is not None:
        roi = RoiMetrics(
            cycle=roi_cycle,
            instret=roi_instret,
            repeat=roi_repeat,
            hpm=roi_hpm,
        )

    return PerfMetrics(path=path, counters=counters, instr=instr, cycle=cycle, ipc=ipc, roi=roi)


def split_version_impl(label: str) -> tuple[str | None, str | None]:
    if label == "oai":
        return "oai", DEFAULT_IMPL["oai"]
    if label == "scalar":
        return "scalar", "mainline"
    if label == "rvv":
        return "rvv", "mainline"
    if label == "ext":
        return "ext", "mainline"
    if "-" not in label:
        return None, None
    version, impl = label.split("-", 1)
    if version not in SUPPORTED_VERSIONS and version != "rvv":
        return None, None
    return version, impl


def infer_version_impl_from_path(path: Path) -> tuple[str | None, str | None]:
    parts = path.parts
    for idx, part in enumerate(parts):
        if not re.fullmatch(r"n[0-9]+", part):
            continue
        if idx + 3 >= len(parts):
            return None, None
        version = parts[idx + 1]
        bucket = parts[idx + 2]
        if bucket not in {"sim", "analysis"}:
            return None, None
        if version not in SUPPORTED_VERSIONS and version != "rvv":
            return None, None
        candidate = parts[idx + 3]
        if candidate.endswith(".report.txt") or candidate.endswith(".analysis.md"):
            implementation = DEFAULT_IMPL.get(version)
        else:
            implementation = candidate
        return version, implementation
    return None, None


def resolve_version_impl(path: Path, label: str) -> tuple[str | None, str | None]:
    version, implementation = infer_version_impl_from_path(path)
    if version is not None and implementation is not None:
        return version, implementation
    return split_version_impl(label)


def scalar_display_name(implementation: str) -> str:
    return f"标量最佳版（`{implementation}`）"


def oai_display_name(implementation: str) -> str:
    return f"OAI 基线版（`{implementation}`）"


def fmt_int(v: int) -> str:
    return f"{v:,}"


def fmt_float(v: float, digits: int = 6) -> str:
    return f"{v:.{digits}f}"


def fmt_pct(v: float) -> str:
    return f"{v * 100:.2f}%"


def rel_change(new_value: float, base_value: float) -> float | None:
    if base_value == 0:
        return None
    return new_value / base_value - 1.0


def fmt_rel_change(new_value: float, base_value: float) -> str:
    value = rel_change(new_value, base_value)
    if value is None:
        return "-"
    return f"{value * 100:+.2f}%"


def fmt_improvement(new_value: float, base_value: float) -> str:
    value = rel_change(new_value, base_value)
    if value is None:
        return "-"
    return f"{(-value) * 100:.2f}%"


def fmt_metric_norm(label: str, metrics: PerfMetrics) -> str:
    if label in {"instrCnt", "cycleCnt", "IPC", "CPI"}:
        return "-"
    if label == "commit_branch_mispredicts / commit_branch_num":
        return fmt_pct(metrics.branch_ratio())
    if label in {
        "commitInstrLoad",
        "commitInstrStore",
    }:
        return fmt_pct(metrics.share_instr(label))
    return fmt_pct(metrics.share_cycle(label))


def metric_value(label: str, metrics: PerfMetrics) -> float:
    if label == "instrCnt":
        return metrics.instr
    if label == "cycleCnt":
        return metrics.cycle
    if label == "IPC":
        return metrics.ipc
    if label == "CPI":
        return metrics.cpi
    if label == "commit_branch_mispredicts / commit_branch_num":
        return metrics.branch_ratio()
    return metrics.get(label)


def fmt_metric_value(label: str, metrics: PerfMetrics) -> str:
    if label in {"IPC", "CPI"}:
        return fmt_float(metric_value(label, metrics), 6)
    if label == "commit_branch_mispredicts / commit_branch_num":
        return (
            f"{metrics.get('commit_branch_mispredicts'):,} / "
            f"{metrics.get('commit_branch_num'):,}"
        )
    return fmt_int(int(metric_value(label, metrics)))


def fmt_avg_cycle_per_repeat(cycle: int, repeat: int) -> str:
    if repeat <= 0:
        return "-"
    return f"{cycle / repeat:,.2f}"


def fmt_avg_instr_per_repeat(instr: int, repeat: int) -> str:
    if repeat <= 0:
        return "-"
    return f"{instr / repeat:,.2f}"


def render_overview_table(rows: list[tuple[str, PerfMetrics]], cycle_base: float, instr_base: float, repeat: int) -> str:
    lines = [
        "| 实现 | instrCnt | cycleCnt | 平均周期/次 | IPC | CPI | 相对基线 cycle 变化 | 相对基线 instr 变化 |",
        "| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |",
    ]
    for name, m in rows:
        lines.append(
            "| "
            + " | ".join(
                [
                    name,
                    fmt_int(m.instr),
                    fmt_int(m.cycle),
                    fmt_avg_cycle_per_repeat(m.cycle, repeat),
                    fmt_float(m.ipc, 6),
                    fmt_float(m.cpi, 6),
                    fmt_rel_change(m.cycle, cycle_base),
                    fmt_rel_change(m.instr, instr_base),
                ]
            )
            + " |"
        )
    return "\n".join(lines)


def render_scalar_overview_table(name: str, metrics: PerfMetrics, repeat: int) -> str:
    if metrics.has_roi():
        roi_repeat = metrics.primary_repeat(repeat)
        return "\n".join(
            [
                "| 实现 | 主口径 | ROI instret | ROI cycle | 平均指令/次 | 平均周期/次 | ROI IPC | ROI CPI | whole instrCnt | whole cycleCnt |",
                "| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |",
                "| "
                + " | ".join(
                    [
                        name,
                        metrics.scope_label(),
                        fmt_int(metrics.primary_instr()),
                        fmt_int(metrics.primary_cycle()),
                        fmt_avg_instr_per_repeat(metrics.primary_instr(), roi_repeat),
                        fmt_avg_cycle_per_repeat(metrics.primary_cycle(), roi_repeat),
                        fmt_float(metrics.primary_ipc(), 6),
                        fmt_float(metrics.primary_cpi(), 6),
                        fmt_int(metrics.instr),
                        fmt_int(metrics.cycle),
                    ]
                )
                + " |",
            ]
        )

    return "\n".join(
        [
            "| 实现 | instrCnt | cycleCnt | 平均指令/次 | 平均周期/次 | IPC | CPI |",
            "| --- | ---: | ---: | ---: | ---: | ---: | ---: |",
            "| "
            + " | ".join(
                [
                    name,
                    fmt_int(metrics.instr),
                    fmt_int(metrics.cycle),
                    fmt_avg_instr_per_repeat(metrics.instr, repeat),
                    fmt_avg_cycle_per_repeat(metrics.cycle, repeat),
                    fmt_float(metrics.ipc, 6),
                    fmt_float(metrics.cpi, 6),
                ]
            )
            + " |",
        ]
    )


def render_roi_hpm_table(metrics: PerfMetrics, repeat: int) -> str:
    roi_repeat = metrics.primary_repeat(repeat)
    lines = [
        "| 分组 | 事件 | ROI 总量 | 平均/次 |",
        "| --- | --- | ---: | ---: |",
    ]
    for (group, event), value in metrics.roi_hpm_items():
        lines.append(
            "| "
            + " | ".join(
                [
                    group,
                    f"`{event}`",
                    fmt_int(value),
                    fmt_avg_cycle_per_repeat(value, roi_repeat),
                ]
            )
            + " |"
        )
    return "\n".join(lines)


def render_single_counter_table(metrics: PerfMetrics) -> str:
    lines = [
        "| 计数器 | 绝对值 | 归一化 |",
        "| --- | ---: | ---: |",
    ]
    for label in COUNTER_LABELS:
        lines.append(
            "| "
            + " | ".join(
                [
                    f"`{label}`",
                    fmt_metric_value(label, metrics),
                    fmt_metric_norm(label, metrics),
                ]
            )
            + " |"
        )
    return "\n".join(lines)


def render_key_counter_table(left_name: str, left: PerfMetrics, right_name: str, right: PerfMetrics, ratio_mode: str) -> str:
    ratio_header = (
        f"{left_name} 相对 {right_name} 变化"
        if ratio_mode == "left_right"
        else f"{right_name} 相对 {left_name} 变化"
    )
    lines = [
        f"| 计数器 | {left_name} 绝对值 | {left_name} 归一化 | {right_name} 绝对值 | {right_name} 归一化 | {ratio_header} |",
        "| --- | ---: | ---: | ---: | ---: | ---: |",
    ]
    for label in COUNTER_LABELS:
        left_val = metric_value(label, left)
        right_val = metric_value(label, right)
        ratio = (
            fmt_rel_change(left_val, right_val)
            if ratio_mode == "left_right"
            else fmt_rel_change(right_val, left_val)
        )
        lines.append(
            "| "
            + " | ".join(
                [
                    f"`{label}`",
                    fmt_metric_value(label, left),
                    fmt_metric_norm(label, left),
                    fmt_metric_value(label, right),
                    fmt_metric_norm(label, right),
                    ratio,
                ]
            )
            + " |"
        )
    return "\n".join(lines)


def refs_text(refs: list[tuple[str, str]]) -> str:
    return "；".join([f"{desc} `{ref}`" for desc, ref in refs])


def render_term_glossary(points: int, include_helper: bool = False) -> str:
    lines = [
        "## 0. 术语说明",
        "",
        "- `leaf` / `leaf kernel`：分治或层级实现里最底层的固定点数计算核；在这组报告里通常就是 16 点核。",
    ]
    if points > 16:
        lines.extend(
            [
                "- `共享框架`：指 16 点以上当前实现复用的外层多点数实现，主要包括 `split`、`combine`、twiddle 复乘和层级拼装。",
                "- `split`：指把一个大点数问题按分解方式拆成多个更小子问题的阶段。",
                "- `combine`：指把这些子问题结果按 `radix2/radix4` 蝶形和 twiddle 复乘重新合并的阶段。",
            ]
        )
    if include_helper:
        lines.append("- `helper`：这里指源码或最终代码形态里的独立辅助函数/例程，不是硬件模块。")
    return "\n".join(lines)


def score_frontend(m: PerfMetrics) -> float:
    return (
        m.share_cycle("stallCycles_fetch_icacheMain")
        + m.share_cycle("stallCycles_decodeFull")
        + m.share_cycle("stallCycles_ibufferFull")
    )


def score_memory(m: PerfMetrics) -> float:
    return (
        m.share_cycle("waitLoadCycle")
        + m.share_cycle("waitStoreCycle")
        + m.share_instr("commitInstrLoad")
        + m.share_instr("commitInstrStore")
    )


def score_control(m: PerfMetrics) -> float:
    return m.branch_ratio() + m.share_cycle("squashCycles_memVio")


def frontend_breakdown_text(m: PerfMetrics) -> str:
    return " / ".join(
        [
            fmt_pct(m.share_cycle("stallCycles_fetch_icacheMain")),
            fmt_pct(m.share_cycle("stallCycles_decodeFull")),
            fmt_pct(m.share_cycle("stallCycles_ibufferFull")),
        ]
    )


def render_scalar_bottlenecks(points: int, scalar: PerfMetrics, oai: PerfMetrics) -> list[tuple[float, str]]:
    shared_hint = (
        "16 点以上的点数里，真正占主体的是共享框架里的多层 split/combine/twiddle 链，"
        f"主要落在 {refs_text(SHARED_REFS)}；`dft16_scalar()` 只负责 leaf 16 点核。"
        if points > 16
        else f"这一点数直接命中 {refs_text(SCALAR_LEAF_REFS)}。"
    )

    front = (
        f"### 3.1 `stallCycles_fetch_icacheMain` + `stallCycles_decodeFull` + `stallCycles_ibufferFull`：前端供给仍然是标量最佳版的第一瓶颈\n\n"
        f"- `stallCycles_fetch_icacheMain = {fmt_int(scalar.get('stallCycles_fetch_icacheMain'))}`，占 `cycleCnt` 的 `{fmt_pct(scalar.share_cycle('stallCycles_fetch_icacheMain'))}`。\n"
        f"- `stallCycles_decodeFull = {fmt_int(scalar.get('stallCycles_decodeFull'))}`，`stallCycles_ibufferFull = {fmt_int(scalar.get('stallCycles_ibufferFull'))}`。\n"
        f"- 相对 OAI，这三个计数器的变化分别是 `{fmt_rel_change(scalar.get('stallCycles_fetch_icacheMain'), oai.get('stallCycles_fetch_icacheMain'))}`、"
        f"`{fmt_rel_change(scalar.get('stallCycles_decodeFull'), oai.get('stallCycles_decodeFull'))}`、"
        f"`{fmt_rel_change(scalar.get('stallCycles_ibufferFull'), oai.get('stallCycles_ibufferFull'))}`。\n\n"
        f"根因优先看代码形态而不是单条算术指令：{shared_hint}"
    )

    memory = (
        f"### 3.2 `commitInstrLoad/Store` + `waitLoad/StoreCycle`：多点数拼装的临时搬运仍然是第二梯队成本\n\n"
        f"- `commitInstrLoad = {fmt_int(scalar.get('commitInstrLoad'))}`，`commitInstrStore = {fmt_int(scalar.get('commitInstrStore'))}`，"
        f"分别占 `instrCnt` 的 `{fmt_pct(scalar.share_instr('commitInstrLoad'))}` 和 `{fmt_pct(scalar.share_instr('commitInstrStore'))}`。\n"
        f"- `waitLoadCycle = {fmt_int(scalar.get('waitLoadCycle'))}`，`waitStoreCycle = {fmt_int(scalar.get('waitStoreCycle'))}`。\n\n"
        f"这类开销如果是 16 点以上，主要不是某一处单独 bug，而是共享框架的共性代价："
        f"{refs_text(SHARED_REFS)} 都会把输入重排、临时 buffer 往返和 twiddle 复乘串起来。"
    )

    control = (
        f"### 3.3 `commit_branch_mispredicts / commit_branch_num` + `CPI`：标量 fixed-point 链的控制与 ILP 还有限制\n\n"
        f"- `IPC = {fmt_float(scalar.ipc, 6)}`，`CPI = {fmt_float(scalar.cpi, 6)}`。\n"
        f"- `commit_branch_mispredicts / commit_branch_num = "
        f"{fmt_int(scalar.get('commit_branch_mispredicts'))} / {fmt_int(scalar.get('commit_branch_num'))}`，"
        f"比率是 `{fmt_pct(scalar.branch_ratio())}`。\n"
        f"- `squashCycles_memVio = {fmt_int(scalar.get('squashCycles_memVio'))}`。\n\n"
        f"这里更像是 fixed-point 饱和/收窄链造成的共性限制，主要对应 "
        f"{refs_text(SCALAR_LEAF_REFS)}，再叠加多点数层级拼装后的循环控制。"
    )

    return [
        (score_frontend(scalar), front),
        (score_memory(scalar), memory),
        (score_control(scalar), control),
    ]


def render_oai_bottlenecks(points: int, oai: PerfMetrics, scalar: PerfMetrics) -> list[tuple[float, str]]:
    shared_hint = (
        "16 点以上的点数里，OAI 和 Scalar 共用同一套多点数 split/combine 框架；"
        "真正额外的差异主要集中在 OAI 的 16 点 leaf packed 语义链。"
        if points > 16
        else "这一点数里几乎全部差异都来自 OAI 的 16 点 packed/SIMDE 语义链。"
    )

    front = (
        f"### 3.1 `stallCycles_fetch_icacheMain` + `stallCycles_decodeFull` + `stallCycles_ibufferFull`：OAI 仍然首先受前端代码流限制\n\n"
        f"- `stallCycles_fetch_icacheMain = {fmt_int(oai.get('stallCycles_fetch_icacheMain'))}`，占 `cycleCnt` 的 `{fmt_pct(oai.share_cycle('stallCycles_fetch_icacheMain'))}`。\n"
        f"- `stallCycles_decodeFull = {fmt_int(oai.get('stallCycles_decodeFull'))}`，`stallCycles_ibufferFull = {fmt_int(oai.get('stallCycles_ibufferFull'))}`。\n"
        f"- 相对 Scalar，这三项的变化分别是 `{fmt_rel_change(oai.get('stallCycles_fetch_icacheMain'), scalar.get('stallCycles_fetch_icacheMain'))}`、"
        f"`{fmt_rel_change(oai.get('stallCycles_decodeFull'), scalar.get('stallCycles_decodeFull'))}`、"
        f"`{fmt_rel_change(oai.get('stallCycles_ibufferFull'), scalar.get('stallCycles_ibufferFull'))}`。\n\n"
        f"根因是两层叠加：一层是共享多点数框架 `{refs_text(SHARED_REFS)}`，"
        f"另一层是 OAI leaf 上的 packed shuffle / unpack / madd 语义链 `{refs_text(OAI_LEAF_REFS)}`。{shared_hint}"
    )

    memory = (
        f"### 3.2 `commitInstrLoad/Store` + `waitLoad/StoreCycle`：packed 语义维护叠加共享 buffer 往返，放大了访存侧成本\n\n"
        f"- `commitInstrLoad = {fmt_int(oai.get('commitInstrLoad'))}`，`commitInstrStore = {fmt_int(oai.get('commitInstrStore'))}`。\n"
        f"- `waitLoadCycle = {fmt_int(oai.get('waitLoadCycle'))}`，`waitStoreCycle = {fmt_int(oai.get('waitStoreCycle'))}`。\n\n"
        f"共享框架本身就需要做 split/combine 和临时 buffer 搬运；而 OAI leaf 还要额外维持 packed lane 语义，"
        f"这部分主要落在 {refs_text(OAI_LEAF_REFS)}。"
    )

    control = (
        f"### 3.3 `instrCnt` + `IPC/CPI`：OAI 的问题核心不是单独一个 IPC，而是为 leaf 语义付出了更多工作量\n\n"
        f"- `instrCnt = {fmt_int(oai.instr)}`，相对 Scalar 的变化是 `{fmt_rel_change(oai.instr, scalar.instr)}`。\n"
        f"- `IPC = {fmt_float(oai.ipc, 6)}`，`CPI = {fmt_float(oai.cpi, 6)}`。\n"
        f"- `commit_branch_mispredicts / commit_branch_num = "
        f"{fmt_int(oai.get('commit_branch_mispredicts'))} / {fmt_int(oai.get('commit_branch_num'))}`，"
        f"比率是 `{fmt_pct(oai.branch_ratio())}`。\n\n"
        f"因此 OAI 这里不要只盯 `IPC` 看，真正更稳定的解释是："
        f"共享框架之外，leaf 16 点核 `{refs_text(OAI_LEAF_REFS)}` 仍然保留了较重的 packed 语义维护。"
    )

    return [
        (score_frontend(oai), front),
        (score_memory(oai), memory),
        (abs(math.log(max(oai.instr, 1) / max(scalar.instr, 1))) + score_control(oai), control),
    ]


def render_comparison_bottlenecks(points: int, oai: PerfMetrics, scalar: PerfMetrics) -> list[tuple[float, str]]:
    shared_hint = (
        "由于两边在 16 点以上共用 `include/fft_pow2_scalar_impl.h` 的多点数框架，"
        "所以点数越大，差异越容易被共性 split/combine/twiddle 开销稀释。"
        if points > 16
        else "这一点数直接对比的就是两种 16 点 leaf kernel。"
    )

    work = (
        f"### 3.1 `instrCnt` + `commitInstrLoad/Store`：两条实现的第一差异来自 leaf kernel 自身要做多少额外工作\n\n"
        f"- `instrCnt`：OAI `{fmt_int(oai.instr)}`，Scalar `{fmt_int(scalar.instr)}`，Scalar 相对 OAI 的变化是 `{fmt_rel_change(scalar.instr, oai.instr)}`。\n"
        f"- `commitInstrLoad + commitInstrStore`：OAI `{fmt_int(oai.get('commitInstrLoad') + oai.get('commitInstrStore'))}`，"
        f"Scalar `{fmt_int(scalar.get('commitInstrLoad') + scalar.get('commitInstrStore'))}`，"
        f"Scalar 相对 OAI 的变化是 `{fmt_rel_change(scalar.get('commitInstrLoad') + scalar.get('commitInstrStore'), oai.get('commitInstrLoad') + oai.get('commitInstrStore'))}`。\n\n"
        f"根因上，Scalar leaf 主要是 `{refs_text(SCALAR_LEAF_REFS)}`；"
        f"OAI leaf 则是 `{refs_text(OAI_LEAF_REFS)}`。{shared_hint}"
    )

    front = (
        f"### 3.2 `stallCycles_fetch_icacheMain` + `stallCycles_decodeFull` + `stallCycles_ibufferFull`：周期差距是否继续放大，取决于前端有没有被 leaf 差异继续放大\n\n"
        f"- `stallCycles_fetch_icacheMain`：OAI `{fmt_int(oai.get('stallCycles_fetch_icacheMain'))}`，Scalar `{fmt_int(scalar.get('stallCycles_fetch_icacheMain'))}`。\n"
        f"- `stallCycles_decodeFull`：OAI `{fmt_int(oai.get('stallCycles_decodeFull'))}`，Scalar `{fmt_int(scalar.get('stallCycles_decodeFull'))}`。\n"
        f"- `stallCycles_ibufferFull`：OAI `{fmt_int(oai.get('stallCycles_ibufferFull'))}`，Scalar `{fmt_int(scalar.get('stallCycles_ibufferFull'))}`。\n\n"
        f"如果这组差距明显，就说明 OAI leaf 的 packed 语义链仍在放大前端压力；"
        f"如果差距收敛，则说明共享框架 `{refs_text(SHARED_REFS)}` 已经开始主导。"
    )

    mem = (
        f"### 3.3 `waitLoad/StoreCycle` + `commit_branch_mispredicts / commit_branch_num`：共享框架之外，谁的副成本更大\n\n"
        f"- `waitLoadCycle`：OAI `{fmt_int(oai.get('waitLoadCycle'))}`，Scalar `{fmt_int(scalar.get('waitLoadCycle'))}`。\n"
        f"- `waitStoreCycle`：OAI `{fmt_int(oai.get('waitStoreCycle'))}`，Scalar `{fmt_int(scalar.get('waitStoreCycle'))}`。\n"
        f"- 分支失预测比率：OAI `{fmt_pct(oai.branch_ratio())}`，Scalar `{fmt_pct(scalar.branch_ratio())}`。\n\n"
        f"当 `Scalar` 相对 `OAI` 的 `cycleCnt` 变化是 `{fmt_rel_change(scalar.cycle, oai.cycle)}` 时，这一组更适合作为“为什么差距还存在/为什么差距没有更大”的辅助解释："
        "leaf kernel 的差别会先体现在工作量和前端，访存等待与控制流则负责把差距继续放大或部分抵消。"
    )

    work_score = abs(math.log(max(oai.instr, 1) / max(scalar.instr, 1))) + abs(
        math.log(
            max(oai.get("commitInstrLoad") + oai.get("commitInstrStore"), 1)
            / max(scalar.get("commitInstrLoad") + scalar.get("commitInstrStore"), 1)
        )
    )
    front_score = abs(score_frontend(oai) - score_frontend(scalar)) + abs(
        math.log(
            max(oai.get("stallCycles_fetch_icacheMain"), 1)
            / max(scalar.get("stallCycles_fetch_icacheMain"), 1)
        )
    )
    mem_score = abs(score_memory(oai) - score_memory(scalar)) + abs(score_control(oai) - score_control(scalar))

    return [(work_score, work), (front_score, front), (mem_score, mem)]


def impl_analysis_filename(points: int, repeat: int, version: str) -> str:
    return f"n{points}-r{repeat}-idft0-{version}.analysis.md"


def cross_version_analysis_filenames(points: int, repeat: int) -> list[str]:
    return [template.format(points=points, repeat=repeat) for template in COMPARISON_REPORT_TEMPLATES]


def point_dir(run_dir: Path, points: int) -> Path:
    return run_dir / f"n{points}"


def impl_analysis_dir(run_dir: Path, points: int, version: str, implementation: str) -> Path:
    default_impl = DEFAULT_IMPL.get(version)
    base = point_dir(run_dir, points) / version / "analysis"
    if implementation == default_impl:
        return base
    return base / implementation


def iter_point_reports(run_dir: Path, points: int | None = None):
    pattern = f"n{points}-r*-idft0-*.report.txt" if points is not None else "n*-r*-idft0-*.report.txt"
    for path in run_dir.rglob(pattern):
        if "backups" in path.parts:
            continue
        match = REPORT_NAME_RE.fullmatch(path.name)
        if not match:
            continue
        version, implementation = resolve_version_impl(path, match.group("label"))
        if version not in {"oai", "scalar"} or implementation is None:
            continue
        yield (
            int(match.group("points")),
            int(match.group("repeat")),
            version,
            implementation,
            path,
        )


def choose_best_scalar_report(candidates: list[tuple[str, Path]]) -> tuple[str, PerfMetrics] | None:
    ranked: list[tuple[int, float, int, str, PerfMetrics]] = []
    for implementation, path in candidates:
        try:
            metrics = parse_report(path)
        except ValueError:
            continue
        ranked.append(
            (
                metrics.cycle,
                -path.stat().st_mtime,
                metrics.instr,
                implementation,
                metrics,
            )
        )
    if not ranked:
        return None
    _, _, _, implementation, metrics = min(ranked)
    return implementation, metrics


def choose_latest_oai_report(candidates: list[tuple[str, Path]]) -> tuple[str, PerfMetrics] | None:
    ranked: list[tuple[float, int, str, PerfMetrics]] = []
    for implementation, path in candidates:
        try:
            metrics = parse_report(path)
        except ValueError:
            continue
        ranked.append((path.stat().st_mtime, -metrics.cycle, implementation, metrics))
    if not ranked:
        return None
    _, _, implementation, metrics = max(ranked)
    return implementation, metrics


def choose_report_pair(run_dir: Path, points: int) -> ReportSelection:
    grouped: dict[int, dict[str, list[tuple[str, Path]]]] = {}
    for _, repeat, version, implementation, path in iter_point_reports(run_dir, points):
        if version == "oai" and implementation != DEFAULT_IMPL["oai"]:
            continue
        grouped.setdefault(repeat, {}).setdefault(version, []).append((implementation, path))

    ranked: list[tuple[float, int, ReportSelection]] = []
    for repeat, paths in grouped.items():
        scalar_choice = choose_best_scalar_report(paths.get("scalar", []))
        oai_choice = choose_latest_oai_report(paths.get("oai", []))
        if scalar_choice is None or oai_choice is None:
            continue
        scalar_impl, scalar = scalar_choice
        oai_impl, oai = oai_choice
        pair_mtime = min(scalar.path.stat().st_mtime, oai.path.stat().st_mtime)
        ranked.append(
            (
                pair_mtime,
                repeat,
                ReportSelection(
                    repeat=repeat,
                    scalar_impl=scalar_impl,
                    scalar=scalar,
                    oai_impl=oai_impl,
                    oai=oai,
                ),
            )
        )

    if ranked:
        _, _, selection = max(ranked)
        return selection

    raise FileNotFoundError(f"no valid scalar/oai pair found for N={points} under {run_dir}")


def discover_candidate_points(run_dir: Path) -> list[int]:
    points = set()
    for points_value, _, version, implementation, _ in iter_point_reports(run_dir):
        if version == "oai" and implementation != DEFAULT_IMPL["oai"]:
            continue
        points.add(points_value)
    return sorted(points)


def discover_valid_pairs(run_dir: Path) -> dict[int, ReportSelection]:
    candidate_points = discover_candidate_points(run_dir)
    if not candidate_points:
        raise FileNotFoundError(f"no oai/scalar perf reports found under {run_dir}")

    results: dict[int, ReportSelection] = {}
    missing_pairs: list[int] = []
    for points in candidate_points:
        try:
            results[points] = choose_report_pair(run_dir, points)
        except FileNotFoundError:
            missing_pairs.append(points)

    if results:
        return results

    points_text = ", ".join(f"N={points}" for points in missing_pairs) or "none"
    raise FileNotFoundError(
        f"no valid scalar/oai pair found under {run_dir}; discovered candidate points: {points_text}"
    )


def choose_best_scalar_report_for_point(run_dir: Path, points: int) -> ScalarSelection:
    ranked: list[tuple[float, float, float, str, int, PerfMetrics]] = []
    canonical_repeat = SCALAR_CANONICAL_REPEAT.get(points)
    for _, repeat, version, implementation, path in iter_point_reports(run_dir, points):
        if version != "scalar":
            continue
        try:
            metrics = parse_report(path)
        except ValueError:
            continue
        if canonical_repeat is not None and repeat != canonical_repeat:
            continue
        repeat_for_metrics = metrics.primary_repeat(repeat)
        avg_cycle = metrics.primary_cycle() / repeat_for_metrics if repeat_for_metrics > 0 else float("inf")
        avg_instr = metrics.primary_instr() / repeat_for_metrics if repeat_for_metrics > 0 else float("inf")
        ranked.append(
            (
                avg_cycle,
                avg_instr,
                -path.stat().st_mtime,
                implementation,
                repeat,
                metrics,
            )
        )

    if not ranked:
        raise FileNotFoundError(
            f"no valid scalar perf report found for N={points} under {run_dir}"
            + (
                f" with canonical repeat r{canonical_repeat}"
                if canonical_repeat is not None
                else ""
            )
        )

    _, _, _, implementation, repeat, metrics = min(ranked)
    return ScalarSelection(repeat=repeat, scalar_impl=implementation, scalar=metrics)


def discover_scalar_candidate_points(run_dir: Path) -> list[int]:
    points = sorted(
        {
            points_value
            for points_value, _, version, _, _ in iter_point_reports(run_dir)
            if version == "scalar"
        }
    )
    if not points:
        raise FileNotFoundError(f"no scalar perf reports found under {run_dir}")
    return points


def discover_best_scalars(run_dir: Path) -> dict[int, ScalarSelection]:
    results: dict[int, ScalarSelection] = {}
    for points in discover_scalar_candidate_points(run_dir):
        results[points] = choose_best_scalar_report_for_point(run_dir, points)
    return results


def render_scalar_only_bottlenecks(points: int, scalar: PerfMetrics) -> list[tuple[float, str]]:
    shared_hint = (
        "16 点以上的热点主体已经不是某一条单独算术指令，而是共享多点数框架里的 split/combine/twiddle 链；"
        f"当前标量路径主要落在 {refs_text(SHARED_REFS)}，`dft16_scalar()` 只负责 leaf 16 点核。"
        if points > 16
        else f"这一点数直接命中 {refs_text(SCALAR_LEAF_REFS)}。"
    )

    front_total = score_frontend(scalar)
    front = (
        f"### 3.1 `stallCycles_fetch_icacheMain` + `stallCycles_decodeFull` + `stallCycles_ibufferFull`：前端供给仍然是当前标量路径的第一瓶颈\n\n"
        f"- `stallCycles_fetch_icacheMain = {fmt_int(scalar.get('stallCycles_fetch_icacheMain'))}`，占 `cycleCnt` 的 `{fmt_pct(scalar.share_cycle('stallCycles_fetch_icacheMain'))}`。\n"
        f"- `stallCycles_decodeFull = {fmt_int(scalar.get('stallCycles_decodeFull'))}`，`stallCycles_ibufferFull = {fmt_int(scalar.get('stallCycles_ibufferFull'))}`。\n"
        f"- 这三项合计占 `cycleCnt` 的 `{fmt_pct(front_total)}`。\n\n"
        f"根因优先看最终代码形态而不是单条算术：{shared_hint}"
    )

    memory_total = score_memory(scalar)
    memory = (
        f"### 3.2 `commitInstrLoad/Store` + `waitLoad/StoreCycle`：输入重排、临时 buffer 往返和 twiddle 复乘仍然是第二梯队成本\n\n"
        f"- `commitInstrLoad = {fmt_int(scalar.get('commitInstrLoad'))}`，`commitInstrStore = {fmt_int(scalar.get('commitInstrStore'))}`，"
        f"分别占 `instrCnt` 的 `{fmt_pct(scalar.share_instr('commitInstrLoad'))}` 和 `{fmt_pct(scalar.share_instr('commitInstrStore'))}`。\n"
        f"- `waitLoadCycle = {fmt_int(scalar.get('waitLoadCycle'))}`，`waitStoreCycle = {fmt_int(scalar.get('waitStoreCycle'))}`。\n"
        f"- 这组 load/store + wait 信号合计的归一化强度是 `{fmt_pct(memory_total)}`。\n\n"
        f"如果点数大于 16，这类开销更像共享框架的共性代价：{refs_text(SHARED_REFS)} 都会把重排、搬运和复乘串起来。"
    )

    control = (
        f"### 3.3 `IPC/CPI` + `commit_branch_mispredicts / commit_branch_num`：fixed-point 饱和/收窄链与循环控制仍在限制 ILP\n\n"
        f"- `IPC = {fmt_float(scalar.ipc, 6)}`，`CPI = {fmt_float(scalar.cpi, 6)}`。\n"
        f"- `commit_branch_mispredicts / commit_branch_num = {fmt_int(scalar.get('commit_branch_mispredicts'))} / {fmt_int(scalar.get('commit_branch_num'))}`，"
        f"比率是 `{fmt_pct(scalar.branch_ratio())}`。\n"
        f"- `squashCycles_memVio = {fmt_int(scalar.get('squashCycles_memVio'))}`。\n\n"
        f"这部分更接近 fixed-point 运算链和循环控制的综合限制，主要对应 {refs_text(SCALAR_LEAF_REFS)}，"
        "再叠加 16 点以上层级拼装后的控制流。"
    )

    return [
        (score_frontend(scalar), front),
        (score_memory(scalar), memory),
        (score_control(scalar), control),
    ]


def render_scalar_only_report(points: int, repeat: int, implementation: str, current: PerfMetrics) -> str:
    current_name = scalar_display_name(implementation)
    ordered = "\n\n".join(
        text for _, text in sorted(render_scalar_only_bottlenecks(points, current), key=lambda item: item[0], reverse=True)
    )
    if points > 16:
        workload_note = (
            f"说明：当前保留的标量最佳实现是 `{implementation}`；16 点以上的外层多点数实现由 `fft_pow2_scalar_impl.h` 里的 split/combine/twiddle 共享框架负责，"
            "本页只讨论当前标量路径自身的计数器形态。"
        )
    else:
        workload_note = (
            f"说明：当前保留的标量最佳实现是 `{implementation}`；这一点数直接命中 16 点标量 leaf kernel，本页只讨论当前标量路径自身的计数器形态。"
        )
    has_roi_hpm = bool(current.roi_hpm_items())

    if current.has_roi():
        metric_note = "- 主表优先使用 `FFT_ROI` 的 batch-level `instret/cycle/IPC`；`emu` 结尾的 whole-program `instrCnt/cycleCnt` 只放在辅助列。"
        repeat_note = "- 下表里 `平均指令/次 = ROI instret / repeat`，`平均周期/次 = ROI cycle / repeat`。"
    else:
        metric_note = "- 当前报告没有 `FFT_ROI` 行，主表仍回退到 whole-program `instrCnt/cycleCnt`。"
        repeat_note = "- 下表里 `平均指令/次 = instrCnt / FFT_REPEAT`，`平均周期/次 = cycleCnt / FFT_REPEAT`。"

    lines = [
        f"# DFT{points} Scalar 最佳版性能分析",
        "",
        f"- 报告文件：`{current.path.name}`",
        f"- 工作负载：`FFT_POINTS={points} FFT_REPEAT={repeat} FFT_RUN_IDFT=0 FFT_VALIDATE=0`",
        f"- {workload_note}",
        metric_note,
        repeat_note,
        "",
        render_term_glossary(points, include_helper=False),
        "",
        "## 1. 总体结果",
        "",
        render_scalar_overview_table(current_name, current, repeat),
        "",
        "- 这页不再保留 OAI 配对结果；如果要做跨点数讨论，优先看主口径的 `平均指令/次`、`平均周期/次`，再结合辅助计数器解释原因。",
        "",
    ]

    if has_roi_hpm:
        lines.extend(
            [
                "## 2. ROI HPM 关键事件",
                "",
                "- 这里只展示当前 build 上 selector 能确认、且对 FFT 分析最关键的 ROI HPM 事件；不支持的事件不写空列。",
                "",
                render_roi_hpm_table(current, repeat),
                "",
                "## 3. Whole-program 关键计数器总览",
                "",
                render_single_counter_table(current),
                "",
                "## 4. 按瓶颈程度展开",
                "",
                ordered,
                "",
            ]
        )
    else:
        lines.extend(
            [
                "## 2. Whole-program 关键计数器总览",
                "",
                render_single_counter_table(current),
                "",
                "## 3. 按瓶颈程度展开",
                "",
                ordered,
                "",
            ]
        )

    return "\n".join(lines)


def render_scalar_only_summary(results: dict[int, ScalarSelection]) -> str:
    points_list = sorted(results)
    any_roi = any(selection.scalar.has_roi() for selection in results.values())
    lines = [
        "# Pow2 标量最佳版跨点数分析",
        "",
        "- 本报告只讨论每个点数当前保留下来的标量最佳版。",
        "- 因为 published 点数混用了不同 `FFT_REPEAT`，跨点数优先看主口径的 `平均指令/次` 和 `平均周期/次`，不要直接拿原始总量横比。",
        (
            "- 有 `FFT_ROI` 的新报告优先使用 batch-level ROI 主口径；没有 ROI 的旧报告继续回退到 whole-program summary，并在 `口径` 列里明确标注。"
            if any_roi
            else "- 下表里 `平均指令/次 = instrCnt / FFT_REPEAT`，`平均周期/次 = cycleCnt / FFT_REPEAT`。"
        ),
        (
            "- 若 `口径=ROI`，则平均值来自 `FFT_ROI`；若 `口径=whole-program`，则平均值来自 `emu` 结尾 summary。"
            if any_roi
            else ""
        ),
        "",
        render_term_glossary(max(points_list) if points_list else 16, include_helper=True),
        "",
        "## 1. 当前保留结果总览",
        "",
        (
            "| 点数 | Repeat | 口径 | 标量最佳版 | 主口径 instr | 主口径 cycle | 平均指令/次 | 平均周期/次 | IPC | fetch/decode/ibuffer 占 cycle | load/store 占 instr | waitLoad/Store 占 cycle | 分支失预测率 |"
            if any_roi
            else "| 点数 | Repeat | 标量最佳版 | instrCnt | cycleCnt | 平均指令/次 | 平均周期/次 | IPC | fetch/decode/ibuffer 占 cycle | load/store 占 instr | waitLoad/Store 占 cycle | 分支失预测率 |"
        ),
        (
            "| ---: | ---: | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |"
            if any_roi
            else "| ---: | ---: | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |"
        ),
    ]

    for points in points_list:
        selection = results[points]
        scalar = selection.scalar
        repeat_for_metrics = scalar.primary_repeat(selection.repeat)
        load_store_share = scalar.share_instr("commitInstrLoad") + scalar.share_instr("commitInstrStore")
        wait_share = scalar.share_cycle("waitLoadCycle") + scalar.share_cycle("waitStoreCycle")
        row = [str(points), str(selection.repeat)]
        if any_roi:
            row.extend(
                [
                    scalar.scope_label(),
                    f"`{selection.scalar_impl}`",
                    fmt_int(scalar.primary_instr()),
                    fmt_int(scalar.primary_cycle()),
                    fmt_avg_instr_per_repeat(scalar.primary_instr(), repeat_for_metrics),
                    fmt_avg_cycle_per_repeat(scalar.primary_cycle(), repeat_for_metrics),
                    fmt_float(scalar.primary_ipc(), 6),
                    frontend_breakdown_text(scalar),
                    fmt_pct(load_store_share),
                    fmt_pct(wait_share),
                    fmt_pct(scalar.branch_ratio()),
                ]
            )
        else:
            row.extend(
                [
                    f"`{selection.scalar_impl}`",
                    fmt_int(scalar.instr),
                    fmt_int(scalar.cycle),
                    fmt_avg_instr_per_repeat(scalar.instr, selection.repeat),
                    fmt_avg_cycle_per_repeat(scalar.cycle, selection.repeat),
                    fmt_float(scalar.ipc, 6),
                    frontend_breakdown_text(scalar),
                    fmt_pct(load_store_share),
                    fmt_pct(wait_share),
                    fmt_pct(scalar.branch_ratio()),
                ]
            )
        lines.append("| " + " | ".join(row) + " |")

    def metric(points: int) -> ScalarSelection:
        return results[points]

    if all(points in results for points in [16, 64, 128, 256]):
        lines.extend(
            [
                "",
                "## 2. 16-256：统一 `mainline` 已经够用，热点还没有演化成必须拆分实现的问题",
                "",
                (
                    f"- `N=16/64/128/256` 当前最佳实现都仍是 `mainline`。"
                    f"`N=16` 的 `IPC` 只有 `{fmt_float(metric(16).scalar.primary_ipc(), 6)}`，但到 `N=64/128/256` 已经抬到 "
                    f"`{fmt_float(metric(64).scalar.primary_ipc(), 6)}` / `{fmt_float(metric(128).scalar.primary_ipc(), 6)}` / `{fmt_float(metric(256).scalar.primary_ipc(), 6)}`。"
                ),
                (
                    f"- 这一段的共同特征是：`N=16` 仍然明显更受固定开销和前端供给影响，"
                    f"但到 `N=64-256` 后，热点形态已经逐步稳定下来，而且当前仍由统一 `mainline` 覆盖。"
                    f"`N=16` 的 `fetch/decode/ibuffer` 占 cycle 分别是 `{frontend_breakdown_text(metric(16).scalar)}`，"
                    f"`N=256` 则是 `{frontend_breakdown_text(metric(256).scalar)}`；"
                    "说明问题更多是在共享框架自然放大，而不是某个顶层 helper 已经必须单独重写。"
                ),
                "- 结论：小点数阶段仍然以 leaf 16 点核加通用 split/combine 框架为主，当前没有证据支持为了 `16-256` 再拆新的专用实现。",
            ]
        )

    if all(points in results for points in [512, 1024]):
        lines.extend(
            [
                "",
                "## 3. 512-1024：共享框架开始主导，baseline 仍坚持同一种固定写法",
                "",
                (
                    f"- `N=512` 和 `N=1024` 当前都统一保留在 `{metric(512).scalar_impl}` / `{metric(1024).scalar_impl}` 主线名下；"
                    f"对应 `平均周期/次` 分别是 `{fmt_avg_cycle_per_repeat(metric(512).scalar.primary_cycle(), metric(512).scalar.primary_repeat(metric(512).repeat))}` 和 "
                    f"`{fmt_avg_cycle_per_repeat(metric(1024).scalar.primary_cycle(), metric(1024).scalar.primary_repeat(metric(1024).repeat))}`。"
                ),
                (
                    f"- 从计数器形态看，`N=512` 到 `N=1024` 的标量路径都已经明显受前端和搬运影响："
                    f"`N=512` 的 `fetch/decode/ibuffer` 占 cycle、`load/store` 占 instr、`wait` 占 cycle 分别是 "
                    f"`{frontend_breakdown_text(metric(512).scalar)}` / `{fmt_pct(metric(512).scalar.share_instr('commitInstrLoad') + metric(512).scalar.share_instr('commitInstrStore'))}` / `{fmt_pct(metric(512).scalar.share_cycle('waitLoadCycle') + metric(512).scalar.share_cycle('waitStoreCycle'))}`，"
                    f"`N=1024` 则是 `{frontend_breakdown_text(metric(1024).scalar)}` / `{fmt_pct(metric(1024).scalar.share_instr('commitInstrLoad') + metric(1024).scalar.share_instr('commitInstrStore'))}` / `{fmt_pct(metric(1024).scalar.share_cycle('waitLoadCycle') + metric(1024).scalar.share_cycle('waitStoreCycle'))}`。"
                ),
                "- 结论：到 `1024` 时，真正能继续带来收益的已经不是再抠 16 点 leaf，而是共享 radix4 combine 热路径的调度与 pipeline 组织；但 baseline 口径仍坚持不用点数专用写法。",
            ]
        )

    if 2048 in results:
        lines.extend(
            [
                "",
                "## 4. 2048：顶层 radix2 combine 成为新的结构性热点",
                "",
                (
                    f"- `N=2048` 当前统一保留在 `{metric(2048).scalar_impl}` 主线名下；`FFT_REPEAT={metric(2048).repeat}`，"
                    "如果该点数已经重跑 ROI，则下文主口径直接对应单次 FFT batch 的真实 kernel 工作量。"
                ),
                (
                    f"- 当前保留下来的 `N=2048` 标量最佳版主口径是 `instr={fmt_int(metric(2048).scalar.primary_instr())}`、"
                    f"`cycle={fmt_int(metric(2048).scalar.primary_cycle())}`、`IPC={fmt_float(metric(2048).scalar.primary_ipc(), 6)}`。"
                ),
                "- 结论：`2048` 的重点不是额外保留一个点数专用 baseline，而是承认顶层 radix2 combine 已经是统一写法下必须观察的结构性热点。",
            ]
        )

    if 4096 in results:
        lines.extend(
            [
                "",
                "## 5. 4096：没有再出现新的低风险 `>5%` 增益点，剩余瓶颈更偏前端/控制流混合",
                "",
                (
                    f"- `N=4096` 当前最佳版仍是 `{metric(4096).scalar_impl}`，"
                    f"对应主口径 `instr={fmt_int(metric(4096).scalar.primary_instr())}`、`cycle={fmt_int(metric(4096).scalar.primary_cycle())}`、`IPC={fmt_float(metric(4096).scalar.primary_ipc(), 6)}`。"
                ),
                "- 已知官方 `emu` 的 `4096` 顶层 pointer-pipeline 跟进实验只把 `cycleCnt` 从 `656,287` 压到 `655,691`，约 `-0.09%`；这远达不到新的一个 `>5%` 级别收益。",
                (
                    f"- 因此 `4096` 当前剩余成本更偏前端/控制流混合：现版 `fetch/decode/ibuffer` 占 cycle 分别是 `{frontend_breakdown_text(metric(4096).scalar)}`，"
                    f"分支失预测率是 `{fmt_pct(metric(4096).scalar.branch_ratio())}`。"
                ),
                "- 结论：当前这版 `4096` 标量已经适合作为收口版本；继续挖也更像是高成本、小收益的组合式调优。",
            ]
        )

    lines.extend(
        [
            "",
            "## 6. 最终结论",
            "",
            "- 跨点数总量如果混用了不同 `FFT_REPEAT`，必须先换算成 `平均指令/次` 和 `平均周期/次`，否则会把 `r10` 与 `r1` 的总数误读成算法工作量差异。",
            "- `16-512`：当前最优解仍然是通用标量框架加已并入主线的收敛优化。",
            "- `1024`：共享 radix4 combine 的 load/use 与 pipeline 组织开始成为主导问题。",
            "- `2048`：热点已经进入顶层 radix2 combine 形态。",
            "- `4096`：当前没有再追一个新 `>5%` 提升的明确低风险入口。",
        ]
    )
    return "\n".join(lines) + "\n"


def render_impl_report(
    points: int,
    repeat: int,
    version: str,
    implementation: str,
    current: PerfMetrics,
    peer_version: str,
    peer_implementation: str,
    peer: PerfMetrics,
) -> str:
    if version == "scalar":
        current_name = scalar_display_name(implementation)
        peer_display_name = oai_display_name(peer_implementation)
        title = f"# DFT{points} Scalar 最佳版性能分析"
        report_name = f"`{current.path.name}`"
        peer_report_name = f"`{peer.path.name}`"
        workload_note = (
            f"说明：当前保留的标量最佳实现是 `{implementation}`；`dft_scalar.c` 只在 16 点 `leaf kernel`（即最底层 16 点计算核）上和 OAI 不同，16 点以上的外层多点数拼装共享同一套实现。"
        )
        entries = render_scalar_bottlenecks(points, current, peer)
        overview = render_overview_table(
            [(current_name, current), (peer_display_name, peer)],
            peer.cycle,
            peer.instr,
            repeat,
        )
        counters = render_key_counter_table("Scalar", current, "OAI", peer, "left_right")
        lead = (
            f"- 当前 {current_name} 相对 {peer_display_name} 的 `cycleCnt` 变化是 `{fmt_rel_change(current.cycle, peer.cycle)}`，"
            f"也就是周期下降了 `{fmt_improvement(current.cycle, peer.cycle)}`。\n"
            f"- 这组结果里，先看 `cycleCnt`，再用 `instrCnt / IPC / 关键 stall` 去解释。"
        )
    else:
        current_name = oai_display_name(implementation)
        peer_display_name = scalar_display_name(peer_implementation)
        title = f"# DFT{points} OAI 基线版性能分析"
        report_name = f"`{current.path.name}`"
        peer_report_name = f"`{peer.path.name}`"
        workload_note = (
            f"说明：当前 OAI 只保留 `oai_dfts.c` 的 `{implementation}` `leaf`（即最底层 16 点计算核），并与 {peer_display_name} 共用同一套多点数拼装实现。"
        )
        entries = render_oai_bottlenecks(points, current, peer)
        overview = render_overview_table(
            [(current_name, current), (peer_display_name, peer)],
            peer.cycle,
            peer.instr,
            repeat,
        )
        counters = render_key_counter_table("OAI", current, "Scalar", peer, "left_right")
        lead = (
            f"- 当前 {current_name} 相对 {peer_display_name} 的 `cycleCnt` 变化是 `{fmt_rel_change(current.cycle, peer.cycle)}`。\n"
            f"- 如果 OAI 仍然落后，主要要从 leaf packed 语义链额外做了多少工作去解释，而不是只看 `IPC`。"
        )

    ordered = "\n\n".join(text for _, text in sorted(entries, key=lambda item: item[0], reverse=True))
    return "\n".join(
        [
            title,
            "",
            f"- 报告文件：{report_name}",
            f"- 对比实现：{peer_display_name}，对应报告 {peer_report_name}",
            f"- 工作负载：`FFT_POINTS={points} FFT_REPEAT={repeat} FFT_RUN_IDFT=0 FFT_VALIDATE=0`",
            f"- {workload_note}",
            "- 下表里 `平均周期/次 = cycleCnt / FFT_REPEAT`。",
            "",
            render_term_glossary(points, include_helper=False),
            "",
            "## 1. 总体结果",
            "",
            overview,
            "",
            lead,
            "",
            "## 2. 关键计数器总览",
            "",
            counters,
            "",
            "## 3. 按瓶颈程度展开",
            "",
            ordered,
            "",
        ]
    )


def render_comparison_report(
    points: int,
    repeat: int,
    oai_impl: str,
    oai: PerfMetrics,
    scalar_impl: str,
    scalar: PerfMetrics,
) -> str:
    scalar_name = scalar_display_name(scalar_impl)
    oai_name = oai_display_name(oai_impl)
    ordered = "\n\n".join(
        text for _, text in sorted(render_comparison_bottlenecks(points, oai, scalar), key=lambda item: item[0], reverse=True)
    )
    return "\n".join(
        [
            f"# DFT{points} OAI 基线 / 标量最佳版 对比分析",
            "",
            f"- 报告文件：`{oai.path.name}`、`{scalar.path.name}`",
            f"- 工作负载：`FFT_POINTS={points} FFT_REPEAT={repeat} FFT_RUN_IDFT=0 FFT_VALIDATE=0`",
            f"- 说明：当前对比固定为 {oai_name} 对 {scalar_name}；这两条路径都按 `rv64gc` 构建并关闭 tree vectorize；16 点以上共用同一套多点数拼装实现。",
            "- 下表里 `平均周期/次 = cycleCnt / FFT_REPEAT`。",
            "",
            render_term_glossary(points, include_helper=False),
            "",
            "## 1. 总体结果",
            "",
            render_overview_table(
                [(oai_name, oai), (scalar_name, scalar)],
                oai.cycle,
                oai.instr,
                repeat,
            ),
            "",
            f"- {scalar_name} 相对 {oai_name} 的 `cycleCnt` 变化是 `{fmt_rel_change(scalar.cycle, oai.cycle)}`，也就是周期下降了 `{fmt_improvement(scalar.cycle, oai.cycle)}`。",
            f"- `instrCnt`、前端 stall 和访存等待共同决定这组差距；单独更高或更低的 `IPC` 不能直接代表优劣。",
            "",
            "## 2. 关键计数器总览",
            "",
            render_key_counter_table("OAI", oai, "Scalar", scalar, "right_left"),
            "",
            "## 3. 按瓶颈程度展开",
            "",
            ordered,
            "",
        ]
    )


def render_summary(results: dict[int, ReportSelection]) -> str:
    points_list = sorted(results)
    repeats = {selection.repeat for selection in results.values()}
    covered_points = " / ".join(str(points) for points in points_list)
    workload_line = (
        "- 性能工作负载：统一使用 `FFT_RUN_IDFT=0 FFT_VALIDATE=0`，但 `FFT_REPEAT` 按点数分档配置。"
        if len(repeats) > 1
        else f"- 性能工作负载：统一使用 `FFT_REPEAT={next(iter(repeats))} FFT_RUN_IDFT=0 FFT_VALIDATE=0`。"
    )

    lines = [
        "# Pow2 DFT OAI / Scalar 总结",
        "",
        f"- 本总结只覆盖当前目录下检测到的有效 OAI / Scalar 成对性能结果：`{covered_points}`。",
        workload_line,
        "- 每个点数都先在同一 `FFT_REPEAT` 下选取 `cycleCnt` 最低的标量实现，再与 `OAI minimal` 成对比较。",
        "- 如果不同点数混用了不同 `FFT_REPEAT`，同一点数下的 `Scalar vs OAI` 对比仍然有效，但跨点数原始总量不能直接横向比较。",
        "- 下表里 `平均周期/次 = cycleCnt / FFT_REPEAT`。",
        "",
        render_term_glossary(max(points_list) if points_list else 16, include_helper=True),
        "",
        "## 1. 各点数总览",
        "",
        "| 点数 | Repeat | 标量最佳实现 | Scalar cycleCnt | Scalar 平均周期/次 | OAI cycleCnt | OAI 平均周期/次 | Scalar 相对 OAI 周期变化 | Scalar IPC | OAI IPC | Scalar 分支失预测率 | OAI 分支失预测率 | Scalar instr | OAI instr |",
        "| ---: | ---: | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |",
    ]

    best_improvement = (-1.0, None)
    worst_improvement = (1e9, None)

    for points in points_list:
        selection = results[points]
        repeat = selection.repeat
        scalar = selection.scalar
        oai = selection.oai
        improvement = -(rel_change(scalar.cycle, oai.cycle) or 0.0)
        lines.append(
            "| "
            + " | ".join(
                [
                    str(points),
                    str(repeat),
                    f"`{selection.scalar_impl}`",
                    fmt_int(scalar.cycle),
                    fmt_avg_cycle_per_repeat(scalar.cycle, repeat),
                    fmt_int(oai.cycle),
                    fmt_avg_cycle_per_repeat(oai.cycle, repeat),
                    fmt_rel_change(scalar.cycle, oai.cycle),
                    fmt_float(scalar.ipc, 6),
                    fmt_float(oai.ipc, 6),
                    fmt_pct(scalar.branch_ratio()),
                    fmt_pct(oai.branch_ratio()),
                    fmt_int(scalar.instr),
                    fmt_int(oai.instr),
                ]
            )
            + " |"
        )
        if improvement > best_improvement[0]:
            best_improvement = (improvement, points)
        if improvement < worst_improvement[0]:
            worst_improvement = (improvement, points)

    lines.extend(
        [
            "",
            "## 2. 总体观察",
            "",
            f"- Scalar 相对 OAI 的最大周期降幅出现在 `N={best_improvement[1]}`，降幅是 `{best_improvement[0] * 100:.2f}%`。",
            f"- Scalar 相对 OAI 的最小周期降幅出现在 `N={worst_improvement[1]}`，降幅是 `{worst_improvement[0] * 100:.2f}%`。",
            "- 当前 `16-4096` 的 scalar baseline 全部统一保留在 `mainline`；`1024/2048/4096` 只允许观察统一写法下的高点数行为，不再引入点数专用 scalar baseline。",
            "- 继续优化大点数时，应优先看共享 split/combine/twiddle 链的前端与搬运成本，而不是回到 16 点 leaf 做同样的微调。",
        ]
    )
    return "\n".join(lines) + "\n"


def render_scalar_best_summary(results: dict[int, ReportSelection]) -> str:
    points_list = sorted(results)
    lines = [
        "# Pow2 标量最佳版跨点数分析",
        "",
        "- 本报告只讨论每个点数当前保留下来的标量最佳版，不使用 `cycle/pt`、`instr/pt` 之类跨点数归一化。",
        "- 跨点数只看三类信息：当前最佳实现是否切换、绝对计数器形态如何变化、同点数下相对 OAI 的周期变化如何变化。",
        "- 下表里 `平均周期/次 = cycleCnt / FFT_REPEAT`。",
        "",
        render_term_glossary(max(points_list) if points_list else 16, include_helper=True),
        "",
        "## 1. 当前保留结果总览",
        "",
        "| 点数 | Repeat | 标量最佳版 | instrCnt | cycleCnt | 平均周期/次 | IPC | fetch/decode/ibuffer 占 cycle | load/store 占 instr | waitLoad/Store 占 cycle | 分支失预测率 | Scalar 相对 OAI 周期变化 |",
        "| ---: | ---: | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |",
    ]

    for points in points_list:
        selection = results[points]
        scalar = selection.scalar
        oai = selection.oai
        load_store_share = scalar.share_instr("commitInstrLoad") + scalar.share_instr("commitInstrStore")
        wait_share = scalar.share_cycle("waitLoadCycle") + scalar.share_cycle("waitStoreCycle")
        lines.append(
            "| "
            + " | ".join(
                [
                    str(points),
                    str(selection.repeat),
                    f"`{selection.scalar_impl}`",
                    fmt_int(scalar.instr),
                    fmt_int(scalar.cycle),
                    fmt_avg_cycle_per_repeat(scalar.cycle, selection.repeat),
                    fmt_float(scalar.ipc, 6),
                    frontend_breakdown_text(scalar),
                    fmt_pct(load_store_share),
                    fmt_pct(wait_share),
                    fmt_pct(scalar.branch_ratio()),
                    fmt_rel_change(scalar.cycle, oai.cycle),
                ]
            )
            + " |"
        )

    def metric(points: int) -> ReportSelection:
        return results[points]

    lines.extend(
        [
            "",
            "## 2. 16-256：统一 `mainline` 已经够用，热点还没有演化成必须拆分实现的问题",
            "",
            (
                f"- `N=16/64/128/256` 当前最佳实现都仍是 `mainline`，"
                f"`IPC` 从 `N=16` 的 `{fmt_float(metric(16).scalar.ipc, 6)}` 逐步降到 `N=256` 的 `{fmt_float(metric(256).scalar.ipc, 6)}`，"
                "但还保持在 4 左右的较高水平。"
            ),
            (
                f"- 这一段的共同特征是：前端压力已经不低，但当前仍由统一 `mainline` 覆盖。"
                f"`N=16` 的 `fetch/decode/ibuffer` 占 cycle 分别是 `{frontend_breakdown_text(metric(16).scalar)}`，"
                f"`N=256` 则是 `{frontend_breakdown_text(metric(256).scalar)}`；"
                "说明问题更多是在共享框架自然放大，而不是某个顶层 helper 已经必须单独重写。"
            ),
            "- 结论：小点数阶段仍然以 leaf 16 点核加通用 split/combine 框架为主，当前没有证据支持为了 `16-256` 再拆新的专用实现。",
            "",
            "## 3. 512-1024：共享框架开始主导，baseline 仍坚持同一种固定写法",
            "",
            (
                f"- `N=512` 和 `N=1024` 当前都统一保留在 `{metric(512).scalar_impl}` / `{metric(1024).scalar_impl}` 主线名下；"
                f"`IPC` 分别是 `{fmt_float(metric(512).scalar.ipc, 6)}` 和 `{fmt_float(metric(1024).scalar.ipc, 6)}`。"
            ),
            (
                f"- 从计数器形态看，`N=512` 到 `N=1024` 的标量路径都已经明显受前端和搬运影响："
                f"`N=512` 的 `fetch/decode/ibuffer` 占 cycle、`load/store` 占 instr、`wait` 占 cycle 分别是 "
                f"`{frontend_breakdown_text(metric(512).scalar)}` / `{fmt_pct(metric(512).scalar.share_instr('commitInstrLoad') + metric(512).scalar.share_instr('commitInstrStore'))}` / `{fmt_pct(metric(512).scalar.share_cycle('waitLoadCycle') + metric(512).scalar.share_cycle('waitStoreCycle'))}`，"
                f"`N=1024` 则是 `{frontend_breakdown_text(metric(1024).scalar)}` / `{fmt_pct(metric(1024).scalar.share_instr('commitInstrLoad') + metric(1024).scalar.share_instr('commitInstrStore'))}` / `{fmt_pct(metric(1024).scalar.share_cycle('waitLoadCycle') + metric(1024).scalar.share_cycle('waitStoreCycle'))}`。"
            ),
            "- 结论：到 `1024` 时，真正能继续带来收益的已经不是再抠 16 点 leaf，而是共享 radix4 combine 热路径的调度与 pipeline 组织；但 baseline 口径仍坚持不用点数专用写法。",
            "",
            "## 4. 2048：顶层 radix2 combine 成为新的结构性热点，但 baseline 仍保持统一写法",
            "",
            (
                f"- `N=2048` 当前仍统一保留在 `{metric(2048).scalar_impl}` 主线名下。"
                f"这说明 `2048` 的下一层热点已经上移到顶层 radix2 combine 本身，而不只是继续吃到 `1024 child` 的继承收益。"
            ),
            (
                f"- 当前保留下来的 `N=2048` 标量最佳版是 `instrCnt={fmt_int(metric(2048).scalar.instr)}`、"
                f"`cycleCnt={fmt_int(metric(2048).scalar.cycle)}`、`IPC={fmt_float(metric(2048).scalar.ipc, 6)}`，"
                f"相对同点数 OAI 的周期变化是 `{fmt_rel_change(metric(2048).scalar.cycle, metric(2048).oai.cycle)}`。"
            ),
            "- 结论：`2048` 的最重要信号不是继续拆一个新实现名，而是承认大点数热点已经进入顶层组合 helper 形态，同时 baseline 仍要保持同一套固定写法。",
            "",
            "## 5. 4096：没有再出现新的低风险 `>5%` 增益点，剩余瓶颈更偏前端/控制流混合",
            "",
            (
                f"- `N=4096` 当前仍统一保留在 `{metric(4096).scalar_impl}` 主线名下，"
                f"对应 `instrCnt={fmt_int(metric(4096).scalar.instr)}`、`cycleCnt={fmt_int(metric(4096).scalar.cycle)}`、`IPC={fmt_float(metric(4096).scalar.ipc, 6)}`。"
            ),
            "- 已知官方 `emu` 的 `4096` 顶层 pointer-pipeline 跟进实验只把 `cycleCnt` 从 `656,287` 压到 `655,691`，约 `-0.09%`；这远达不到新的一个 `>5%` 级别收益。",
            (
                f"- 因此 `4096` 现在更合理的结论不是“还差一个明显的大热点没打中”，"
                f"而是当前剩余成本已经更偏前端/控制流混合：现版 `fetch/decode/ibuffer` 占 cycle 分别是 `{frontend_breakdown_text(metric(4096).scalar)}`，"
                f"分支失预测率是 `{fmt_pct(metric(4096).scalar.branch_ratio())}`。"
            ),
            "- 结论：当前这版 `4096` 标量已经适合作为收口版本；继续挖也更像是高成本、小收益的组合式调优，而不是再找一个低风险的大台阶。",
            "",
            "## 6. 最终结论",
            "",
            "- `16-512`：统一 `mainline` 仍然足够，说明这段范围的 baseline 还不需要点数专用写法。",
            "- `1024`：关键问题是共享 radix4 combine 的 load/use 与 pipeline 组织；这属于统一 baseline 下的热点观察，而不是单独放宽优化口径的理由。",
            "- `2048`：热点已经进入顶层 radix2 combine 形态；但 baseline 仍应保持同一套固定写法，避免对高点数额外偏袒。",
            "- `4096`：已知后续专门化尝试只有约 `0.09%` 收益；这也进一步说明没必要为了 baseline 再扩增点数专门版本。",
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
    results = discover_best_scalars(run_dir)
    for points, selection in results.items():
        repeat = selection.repeat
        scalar = selection.scalar
        scalar_analysis_dir = impl_analysis_dir(run_dir, points, "scalar", selection.scalar_impl)
        scalar_analysis_dir.mkdir(parents=True, exist_ok=True)
        (scalar_analysis_dir / impl_analysis_filename(points, repeat, "scalar")).write_text(
            render_scalar_only_report(points, repeat, selection.scalar_impl, scalar)
        )

    summary_dir = scalar_summary_output_dir(run_dir)
    summary_dir.mkdir(parents=True, exist_ok=True)
    (summary_dir / "pow2-scalar-best-summary.analysis.md").write_text(render_scalar_only_summary(results))


if __name__ == "__main__":
    main()
