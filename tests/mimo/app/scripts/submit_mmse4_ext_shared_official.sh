#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
APP=${ROOT}/nexus-am/apps/mimo
validate_tol=${MIMO_VALIDATE_TOL:-7}
ext_mmse4_rvv_i32_binv_reuse=${MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE:-0}
ext_mmse4_rvv_i32_packed_phase=${MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE:-0}
ext_mmse4_packed_top_ext=${MIMO_EXT_MMSE4_PACKED_TOP_EXT:-1}
ext_mmse4_packed_ssolve_ext=${MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT:-0}
ext_mmse4_packed_live_build=${MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD:-0}
ext_mmse4_packed_live_build_unroll4=${MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4:-0}
ext_mmse4_packed_invcoeff_ext=${MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT:-0}
ext_mmse4_packed_vset_chain=${MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN:-0}
ext_mmse4_packed_solve_seed=${MIMO_EXT_MMSE4_PACKED_SOLVE_SEED:-0}
ext_mmse4_packed_det_ext=${MIMO_EXT_MMSE4_PACKED_DET_EXT:-0}
ext_mmse4_packed_fixed_ptrs=${MIMO_USE_EXT_MMSE4_PACKED_FIXED_PTRS:-0}
ext_mmse4_packed_e16_recip=${MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP:-0}
ext_mmse4_packed_e16_lambda=${MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA:-0}
ext_mmse4_packed_rx4_m4_load=${MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD:-0}
ext_zf4_vscmacc_seed_build=${MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD:-0}

cd "${ROOT}"
source ./env.sh >/dev/null
cd "${APP}"

env \
  TIMEOUT_SEC=0 \
  MIMO_MMSE4_EXT_ACTION=submit \
  MIMO_VALIDATE_TOL="${validate_tol}" \
  MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE=0 \
  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE=0 \
  MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE=1 \
  MIMO_USE_EXT_MMSE4_RVV_I32_INNER_Q30_ONCE=1 \
  MIMO_USE_EXT_MMSE4_RVV_I32_TOP_Q30_ONCE=1 \
  MIMO_USE_EXT_MMSE4_RVV_I32_DIAG_VLSE16_LOAD=0 \
  MIMO_USE_EXT_MMSE4_RVV_I32_ASSUME_LAMBDA_NOSAT=1 \
  MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE="${ext_mmse4_rvv_i32_binv_reuse}" \
  MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE="${ext_mmse4_rvv_i32_packed_phase}" \
  MIMO_EXT_MMSE4_PACKED_TOP_EXT="${ext_mmse4_packed_top_ext}" \
  MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT="${ext_mmse4_packed_ssolve_ext}" \
  MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD="${ext_mmse4_packed_live_build}" \
  MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4="${ext_mmse4_packed_live_build_unroll4}" \
  MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT="${ext_mmse4_packed_invcoeff_ext}" \
  MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN="${ext_mmse4_packed_vset_chain}" \
  MIMO_EXT_MMSE4_PACKED_SOLVE_SEED="${ext_mmse4_packed_solve_seed}" \
  MIMO_EXT_MMSE4_PACKED_DET_EXT="${ext_mmse4_packed_det_ext}" \
  MIMO_USE_EXT_MMSE4_PACKED_FIXED_PTRS="${ext_mmse4_packed_fixed_ptrs}" \
  MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP="${ext_mmse4_packed_e16_recip}" \
  MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA="${ext_mmse4_packed_e16_lambda}" \
  MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD="${ext_mmse4_packed_rx4_m4_load}" \
  MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD="${ext_zf4_vscmacc_seed_build}" \
  MIMO_USE_RVV_SCHUR4_NUM_MAC=1 \
  MIMO_USE_RVV_SCHUR4_COMPLEX_MAC=1 \
  MIMO_USE_RVV_SCHUR4_VSSRA_ROUND=1 \
  MIMO_USE_RVV_SCHUR4_DET_MAC=1 \
  bash scripts/run_mmse4_ext_current_best_gate.sh mmse4-ext-shared-official
