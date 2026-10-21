#!/usr/bin/env bash
set -euo pipefail

APP="${MIMO_APP_DIR:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)}"
TIMEOUT_SEC=${TIMEOUT_SEC:-900}
TAG=${1:-zf4-ext-packed-current-best}

cd "${APP}"

env \
  TIMEOUT_SEC="${TIMEOUT_SEC}" \
  MIMO_EQ4_KERNEL=eq_zf_4x4 \
  MIMO_VALIDATE_TOL="${MIMO_VALIDATE_TOL:-11}" \
  MIMO_USE_EXT_ZF4_PACKED_LIVE_BUILD=1 \
  MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE=1 \
  MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE=1 \
  MIMO_EXT_MMSE4_PACKED_TOP_EXT=1 \
  MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT=1 \
  MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD=1 \
  MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4=1 \
  MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT=1 \
  MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN=1 \
  MIMO_EXT_MMSE4_PACKED_SOLVE_SEED=1 \
  MIMO_EXT_MMSE4_PACKED_DET_EXT=1 \
  MIMO_USE_EXT_MMSE4_PACKED_FIXED_PTRS=1 \
  MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP=1 \
  MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA=1 \
  MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD=1 \
  MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE=0 \
  MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD=1 \
  MIMO_USE_RVV_SCHUR4_NUM_MAC=1 \
  MIMO_USE_RVV_SCHUR4_COMPLEX_MAC=1 \
  MIMO_USE_RVV_SCHUR4_VSSRA_ROUND=1 \
  MIMO_USE_RVV_SCHUR4_DET_MAC=1 \
  bash scripts/run_mmse4_ext_current_best_gate.sh "${TAG}"

if [[ "${MIMO_GATE_SKIP_ASM:-0}" == 1 ]]; then
  echo "MIMO_ZF4_EXT_PACKED_HOT_GATE_PASS tag=${TAG} asm=skipped"
  exit 0
fi

asm="${APP}/perf_eval/asm/mimo-ext-riscv64-xs-${TAG}.txt"
counts=$(
  env \
    MIMO_ASM_FORBID_HARD_DIV=1 \
    MIMO_ASM_FORBID_SEGMENT_LDST=1 \
    MIMO_ASM_MIN_CUSTOM_EXT_VIMAC=96 \
    MIMO_ASM_MIN_VLE16=5 \
    MIMO_ASM_MAX_VLE16=5 \
    MIMO_ASM_MIN_E16_M4_VSET=1 \
    MIMO_ASM_MAX_E16_M4_VSET=1 \
    MIMO_ASM_MIN_RX4_M4_GROUP_LOAD=5 \
    MIMO_ASM_MAX_RX4_M4_GROUP_LOAD=5 \
    MIMO_ASM_MAX_STACK_LOAD=0 \
    MIMO_ASM_MAX_STACK_STORE=0 \
    MIMO_ASM_MAX_WHOLE_VREG_LOAD=0 \
    MIMO_ASM_MAX_WHOLE_VREG_STORE=0 \
    MIMO_ASM_MAX_V30_REFS=0 \
    bash scripts/check_asm_symbol_counts.sh \
      --symbol mimo_ext_zf4_packed_live_run "${asm}"
)
printf '%s\n' "${counts}"

for expected in \
  'lines=160' \
  'custom_ext_vimac_any_word=96' \
  'segment_load_store=0' \
  'stack_load=0' \
  'stack_store=0' \
  'whole_vreg_load=0' \
  'whole_vreg_store=0' \
  'v30_refs=0'; do
  if ! grep -Fxq -- "${expected}" <<<"${counts}"; then
    echo "ZF4 packed asm gate failed: expected ${expected}" >&2
    exit 3
  fi
done

hot=$(awk '/<mimo_ext_zf4_packed_live_run>:/,/^$/' "${asm}")
if grep -Eq '[[:space:]]vadd\.vv[[:space:]]' <<<"${hot}"; then
  echo "ZF4 packed asm gate failed: MMSE lambda vadd.vv remains in the ZF hot path." >&2
  exit 3
fi

first_vset=$(awk '
  /<mimo_ext_zf4_packed_live_run>:/ { in_symbol = 1; next }
  in_symbol && /vsetvli[[:space:]].*e16,m4/ {
    sub(/:.*/, "", $1); print $1; exit
  }
' "${asm}")
if [[ -z "${first_vset}" || $((16#${first_vset} % 16)) -ne 0 ]]; then
  echo "ZF4 packed asm gate failed: first e16,m4 vsetvli is not 16-byte aligned (${first_vset:-missing})." >&2
  exit 3
fi

echo "MIMO_ZF4_EXT_PACKED_HOT_GATE_PASS tag=${TAG} first_e16_m4_vset=0x${first_vset}"
