#!/usr/bin/env bash
# =============================================================================
# 05 烧录比特流到 FPGA（JTAG; 约 3 分钟）
#   比特流来源: build/fpga_top_debug.bit（本机构建, 或他人提供的 Releases 产物放到 build/）
#   替代方式: AEConsole 的 FPGA Bin Download（需厂商 BSP, 见 docs/onboard-runbook.md）
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true

[ -f "$BUILD_ROOT/fpga_top_debug.bit" ] || {
    echo "ERROR: 未找到 $BUILD_ROOT/fpga_top_debug.bit"
    echo "  自行构建: flows/04_build_bitstream.sh（约 7 小时）"
    echo "  或把已获得的较比特流 + .ltx 放入 build/ 后重跑本步骤"
    exit 1
}

echo "[05] 烧录 build/fpga_top_debug.bit → FPGA"
vivado -mode tcl -source "$ROOT/tools/prog_bitstream.tcl"

echo
echo "==== 烧录完成 ===="
echo "下一步: flows/06_run_hello.sh（裸机 Hello）或 flows/07_run_linux.sh（Linux 镜像）"
