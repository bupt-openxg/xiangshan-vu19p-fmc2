#!/usr/bin/env bash
# =============================================================================
# 08 板卡诊断比特流（可选; 独立小工程, 综合实现约 20 分钟）
#   用途: 上板前/排障时分钟级验证 —— 四路时钟存活、三个复位引脚电平、
#         VIO 复位释放逻辑、串口链路端到端（VIO 发字符 → 主机收到）
#   详情: hw/README.md §4 与 docs/board-diag.md（含实测结论）
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true
mkdir -p "$BUILD_ROOT/logs"

echo "[08] 构建诊断比特流（hw/diag/）"
vivado -mode batch -source "$ROOT/hw/diag/build_diag.tcl" -log "$BUILD_ROOT/logs/diag_build.log"
echo "[08] 上板运行诊断（编程 + 读探针 + UART 自检）"
vivado -mode tcl -source "$ROOT/hw/diag/scripts/run_diag.tcl"

echo "==== 诊断完成（输出见上面 TCL 日志）===="
