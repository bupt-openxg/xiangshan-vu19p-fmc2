#!/usr/bin/env bash
# =============================================================================
# 阶段4: 对已创建的 Vivado 工程直接启动 综合->实现->写比特流
#   (跳过 `make bitstream` 会重新创建工程的行为; 等价于 make bitstream 的 gen_bitstream 步骤)
#   用法: VIVADO_JOBS=8 tools/run_bitstream.sh
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh"
PRJ_NAME=fpga_vivado_kmh-nodiff
PRJ="$FPGA_ROOT/$PRJ_NAME/$PRJ_NAME.xpr"
LOG="$ROOT/logs/bitstream-run-$(date +%Y%m%d-%H%M).log"
mkdir -p "$ROOT/logs"

[ -f "$PRJ" ] || { echo "ERROR: 工程不存在: $PRJ (先跑 make project)"; exit 1; }
echo "[run_bitstream] 工程: $PRJ"
echo "[run_bitstream] 日志: $LOG"
echo "[run_bitstream] JOBS: ${VIVADO_JOBS:-8}"

cd "$FPGA_ROOT"
vivado -mode batch -source vivado/scripts/gen_bitstream.tcl \
    -tclargs "$PRJ" impl_1 "${VIVADO_JOBS:-8}" 2>&1 | tee "$LOG"
