#!/usr/bin/env bash
# =============================================================================
# 04 创建 Vivado 工程 → 综合 → 实现 → 写比特流   （约 7 小时）
#   产物: build/fpga_top_debug.bit + .ltx（后续烧录用）
#   说明: 综合峰值内存很大（虚拟内存 ~30GB），请保证 swap ≥32GB（见 00/03 检查）
#   注: 若中途意外中断, 用 tools/rerun_bitstream.sh 重置运行后重跑（勿 rm 工程目录）
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true

[ -d "$NOOP_HOME/build/rtl" ] || { echo "ERROR: 未找到 RTL（$NOOP_HOME/build/rtl）"; echo "  先运行 flows/03_gen_rtl.sh"; exit 1; }
mkdir -p "$BUILD_ROOT" "$ROOT/logs"

cd "$FPGA_ROOT"
echo "[04] 创建 Vivado 工程（CPU=kmh NO_DIFF=1 DDR_RANK_WIDTH=1）"
make project CPU=kmh NO_DIFF=1 DDR_RANK_WIDTH=1 CORE_DIR="$NOOP_HOME/build"

echo "[04] 综合 → 实现 → 比特流（VIVADO_JOBS=${VIVADO_JOBS:-8}，日志 logs/）"
VIVADO_JOBS="${VIVADO_JOBS:-8}" bash "$ROOT/tools/run_bitstream.sh"

echo "[04] 归档产物 → build/"
make stage_bitstream CPU=kmh NO_DIFF=1 FPGA_BIT_ARTIFACT_DIR="$BUILD_ROOT"
ls -la "$BUILD_ROOT"/fpga_top_debug.bit "$BUILD_ROOT"/fpga_top_debug.ltx

echo
echo "==== 比特流就绪 ===="
echo "下一步: flows/05_program_fpga.sh（烧录到板卡）"
