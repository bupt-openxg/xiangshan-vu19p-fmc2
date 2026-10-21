#!/usr/bin/env bash
# =============================================================================
# 05 烧录比特流到 FPGA（JTAG; 约 3 分钟）
#   比特流来源: ${FPGA_BIT_HOME:-build}/fpga_top_debug.bit + .ltx
#   综合机构建产物回传后，用 FPGA_BIT_HOME="$BIT_DIR" 指定。
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true

BIT_HOME="${FPGA_BIT_HOME:-$BUILD_ROOT}"
[ -f "$BIT_HOME/fpga_top_debug.bit" ] || {
    echo "ERROR: 未找到 $BIT_HOME/fpga_top_debug.bit"
    echo "  先用 flows/04_package_server.sh 打包，在综合机构建后把 bit/ltx 回传"
    echo "  或设置 FPGA_BIT_HOME 指向已有 bit/ltx 的目录"
    exit 1
}

echo "[05] 烧录 $BIT_HOME/fpga_top_debug.bit → FPGA"
LOG="${BUILD_ROOT}/logs/program-$(date +%Y%m%d-%H%M).log"
mkdir -p "$(dirname "$LOG")"
rc=0
vivado -mode tcl -source "$ROOT/tools/prog_bitstream.tcl" 2>&1 | tee "$LOG" || rc=$?
# vivado -mode tcl 在 Tcl 报错时不一定返回非零, 以脚本成功标记为准
if [ "$rc" -ne 0 ] || ! grep -q "=== 比特流烧录完成 ===" "$LOG"; then
    echo
    echo "ERROR: 烧录失败（vivado 退出码 $rc，日志: $LOG）"
    exit 1
fi

echo
echo "==== 烧录完成 ===="
echo "下一步: tests/bupt/ddr16_check.tcl；然后 flows/10_run_bupt.sh"
