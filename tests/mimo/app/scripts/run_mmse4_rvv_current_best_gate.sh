#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
APP=${ROOT}/nexus-am/apps/mimo
TIMEOUT_SEC=${TIMEOUT_SEC:-900}
TAG=${1:-mmse4-rvv-current-best}

validate_tol=${MIMO_VALIDATE_TOL:-11}
mmse4_schur_num_arshift_scale=${MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE:-1}
mmse4_schur_inner_q30_once=${MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE:-1}
rvv_mmse4_schur_top_q30_once=${MIMO_USE_RVV_MMSE4_SCHUR_TOP_Q30_ONCE:-1}
rvv_mmse4_staged_build_solve=${MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE:-1}
rvv_mmse4_early_tail_rhs=${MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS:-0}
rvv_mmse4_const_vl=${MIMO_USE_RVV_MMSE4_CONST_VL:-0}
rvv_mmse4_seed_acc=${MIMO_USE_RVV_MMSE4_SEED_ACC:-1}
rvv_mmse4_ri_packed_load=${MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD:-0}
rvv_mmse4_binv_q13_reuse=${MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE:-0}
rvv_mmse4_schur_true16b_solve=${MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SOLVE:-0}
rvv_mmse4_schur_true16b_shared_a16=${MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SHARED_A16:-0}
rvv_schur4_solve2_pair_m2=${MIMO_USE_RVV_SCHUR4_SOLVE2_PAIR_M2:-0}
rvv_schur4_vsmul_scale=${MIMO_USE_RVV_SCHUR4_VSMUL_SCALE:-0}
rvv_schur4_num_mac=${MIMO_USE_RVV_SCHUR4_NUM_MAC:-1}
rvv_schur4_complex_mac=${MIMO_USE_RVV_SCHUR4_COMPLEX_MAC:-1}
rvv_schur4_vssra_round=${MIMO_USE_RVV_SCHUR4_VSSRA_ROUND:-1}
rvv_site4_vssra_round=${MIMO_USE_RVV_SITE4_VSSRA_ROUND:-0}
rvv_site4_seg2_load=${MIMO_USE_RVV_SITE4_SEG2_LOAD:-0}
rvv_site4_ri_soa=${MIMO_USE_RVV_SITE4_RI_SOA:-1}
rvv_schur4_det_mac=${MIMO_USE_RVV_SCHUR4_DET_MAC:-1}
gate_skip_nemu=${MIMO_GATE_SKIP_NEMU:-0}
gate_skip_asm=${MIMO_GATE_SKIP_ASM:-0}
gate_allow_emu_running=${MIMO_GATE_ALLOW_EMU_RUNNING:-0}
gate_asm_expect=${MIMO_GATE_ASM_EXPECT:-}
gate_asm_symbol=${MIMO_GATE_ASM_SYMBOL:-}

cd "${ROOT}"
source ./env.sh >/dev/null
cd "${APP}"

while IFS='=' read -r name _; do
  case "${name}" in
    MIMO_*) unset "${name}" ;;
  esac
done < <(env)

COMMON_ENV=(
  TIMEOUT_SEC="${TIMEOUT_SEC}"
  MIMO_GATE_KERNEL=eq_mmse_4x4
  MIMO_VALIDATE_KERNEL=eq_mmse_4x4
  MIMO_PERF_KERNEL=eq_mmse_4x4
  MIMO_VALIDATE_TOL="${validate_tol}"
  MIMO_VALIDATE_ERROR_STATS=1
  MIMO_USE_RECIP_TABLE=0
  MIMO_USE_SOLVE4_HERMITIAN=1
  MIMO_USE_SOLVE4_SCHUR=1
  MIMO_USE_SOLVE4_SCHUR_Q15_RECIP=5
  MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT=13
  MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE=1
  MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE=1
  MIMO_USE_RVV_SITE4_MF2=1
  MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE=1
  MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE=0
  MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE="${mmse4_schur_num_arshift_scale}"
  MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE="${mmse4_schur_inner_q30_once}"
  MIMO_USE_RVV_MMSE4_SCHUR_TOP_Q30_ONCE="${rvv_mmse4_schur_top_q30_once}"
  MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE="${rvv_mmse4_staged_build_solve}"
  MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS="${rvv_mmse4_early_tail_rhs}"
  MIMO_USE_RVV_MMSE4_CONST_VL="${rvv_mmse4_const_vl}"
  MIMO_USE_RVV_MMSE4_SEED_ACC="${rvv_mmse4_seed_acc}"
  MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD="${rvv_mmse4_ri_packed_load}"
  MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE="${rvv_mmse4_binv_q13_reuse}"
  MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SOLVE="${rvv_mmse4_schur_true16b_solve}"
  MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SHARED_A16="${rvv_mmse4_schur_true16b_shared_a16}"
  MIMO_USE_RVV_SCHUR4_SOLVE2_PAIR_M2="${rvv_schur4_solve2_pair_m2}"
  MIMO_USE_RVV_SCHUR4_VSMUL_SCALE="${rvv_schur4_vsmul_scale}"
  MIMO_USE_RVV_SCHUR4_NUM_MAC="${rvv_schur4_num_mac}"
  MIMO_USE_RVV_SCHUR4_COMPLEX_MAC="${rvv_schur4_complex_mac}"
  MIMO_USE_RVV_SCHUR4_VSSRA_ROUND="${rvv_schur4_vssra_round}"
  MIMO_USE_RVV_SITE4_VSSRA_ROUND="${rvv_site4_vssra_round}"
  MIMO_USE_RVV_SITE4_SEG2_LOAD="${rvv_site4_seg2_load}"
  MIMO_USE_RVV_SITE4_RI_SOA="${rvv_site4_ri_soa}"
  MIMO_USE_RVV_SCHUR4_DET_MAC="${rvv_schur4_det_mac}"
  MIMO_USE_SITE4_SOLVE_SOA=1
  MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD=1
  MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE=1
  MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED=0
  MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE=1
  MIMO_USE_SITE4_ROUND_ONCE_DOT=1
  MIMO_EMU_REQUIRE_NO_HARD_DIV=1
  MIMO_ASM_FORBID_HARD_DIV=1
  MIMO_ASM_FORBID_SEGMENT_LDST=1
  MIMO_GATE_SKIP_NEMU="${gate_skip_nemu}"
  MIMO_GATE_SKIP_ASM="${gate_skip_asm}"
  MIMO_GATE_ALLOW_EMU_RUNNING="${gate_allow_emu_running}"
  MIMO_GATE_ASM_EXPECT="${gate_asm_expect}"
  MIMO_GATE_ASM_SYMBOL="${gate_asm_symbol}"
)

echo "MIMO_MMSE4_RVV_CURRENT_BEST_GATE_START tag=${TAG} timeout_sec=${TIMEOUT_SEC} tol=${validate_tol}"
env "${COMMON_ENV[@]}" bash scripts/run_candidate_gates.sh "${TAG}" rvv
echo "MIMO_MMSE4_RVV_CURRENT_BEST_GATE_DONE tag=${TAG}"
