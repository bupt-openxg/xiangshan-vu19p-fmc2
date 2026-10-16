#!/usr/bin/env bash
# =============================================================================
# 修复后重跑: 重置 synth_1/impl_1 再启动完整 综合->实现->比特流
#   用法: VIVADO_JOBS=8 tools/rerun_bitstream.sh
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh"
cd "$FPGA_ROOT"

cat > /tmp/reset_runs.tcl <<'EOF'
open_project fpga_vivado_kmh-nodiff/fpga_vivado_kmh-nodiff.xpr
reset_run synth_1
puts "INFO: synth_1 reset"
if {[get_runs -quiet impl_1] ne ""} {
    reset_run impl_1
    puts "INFO: impl_1 reset"
}
EOF
vivado -mode batch -nojournal -nolog -source /tmp/reset_runs.tcl 2>&1 | grep -E "INFO:|ERROR" | head -5

exec env VIVADO_JOBS="${VIVADO_JOBS:-8}" bash "$ROOT/tools/run_bitstream.sh"
