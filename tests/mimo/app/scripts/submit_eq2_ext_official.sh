#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
APP=${ROOT}/nexus-am/apps/mimo
KERNEL=${1:-eq_zf_2x2}
MODE=${2:-m2}
TIMEOUT_SEC=${TIMEOUT_SEC:-0}
eq2_zf_direct_h=${MIMO_USE_EQ2_ZF_DIRECT_H:-0}
ext_eq2_zf_guard1_hybrid=${MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID:-0}
ext_eq2_rhs_interleave=${MIMO_USE_EXT_EQ2_RHS_INTERLEAVE:-0}
ext_eq2_mmse_adaptive_q13=${MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13:-0}
ext_eq2_invcoeff=1

case "${KERNEL}" in
  eq_zf_2x2|eq_mmse_2x2) ;;
  *) echo "Unsupported EQ2 EXT kernel: ${KERNEL}" >&2; exit 2 ;;
esac
case "${MODE}" in
  m1) ext_eq2_m2=0 ;;
  m2) ext_eq2_m2=1 ;;
  *) echo "Unsupported EQ2 EXT mode: ${MODE}; use m1 or m2." >&2; exit 2 ;;
esac

if [[ "${eq2_zf_direct_h}" == "1" ]]; then
  if [[ "${KERNEL}" != "eq_zf_2x2" || "${MODE}" != "m2" ]]; then
    echo "EQ2 direct-H EXT official requires eq_zf_2x2 m2." >&2
    exit 2
  fi
  if [[ "${ext_eq2_rhs_interleave}" != "0" ]]; then
    echo "EQ2 direct-H EXT official does not use the Gramian RHS-interleave knob." >&2
    exit 2
  fi
  ext_eq2_invcoeff=0
fi

if [[ "${ext_eq2_zf_guard1_hybrid}" != "0" &&
      "${ext_eq2_zf_guard1_hybrid}" != "1" ]]; then
  echo "MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID must be 0 or 1." >&2
  exit 2
fi
if [[ "${ext_eq2_zf_guard1_hybrid}" == "1" &&
      ( "${KERNEL}" != "eq_zf_2x2" || "${MODE}" != "m2" ||
        "${eq2_zf_direct_h}" != "1" || "${ext_eq2_invcoeff}" != "0" ||
        "${ext_eq2_rhs_interleave}" != "0" ) ]]; then
  echo "EXT ZF2 guard1 hybrid official requires direct-H m2 with no invcoeff/RHS interleave." >&2
  exit 2
fi
if [[ "${ext_eq2_mmse_adaptive_q13}" != "0" &&
      "${ext_eq2_mmse_adaptive_q13}" != "1" ]]; then
  echo "MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13 must be 0 or 1." >&2
  exit 2
fi
if [[ "${ext_eq2_mmse_adaptive_q13}" == "1" &&
      ( "${KERNEL}" != "eq_mmse_2x2" ||
        "${ext_eq2_invcoeff}" != "1" || "${ext_eq2_rhs_interleave}" != "0" ) ]]; then
  echo "EXT adaptive Q13 official requires MMSE2 invcoeff." >&2
  exit 2
fi

if [[ "${TIMEOUT_SEC}" != "0" ]]; then
  echo "Refusing EQ2 EXT official: TIMEOUT_SEC must be 0." >&2
  exit 2
fi

cd "${ROOT}"
source ./env.sh >/dev/null
cd "${APP}"

while IFS='=' read -r name _; do
  case "${name}" in MIMO_*) unset "${name}" ;; esac
done < <(env)

env \
  MIMO_WARMUP_REPEAT=0 \
  MIMO_REPEAT=1 \
  MIMO_VALIDATE_TOL=11 \
  MIMO_VALIDATE_ERROR_STATS=1 \
  MIMO_EQ2_RECIP_MODE=5 \
  MIMO_EQ2_RECIP_NUM_SHIFT=13 \
  MIMO_USE_EQ2_ZF_DIRECT_H="${eq2_zf_direct_h}" \
  MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID="${ext_eq2_zf_guard1_hybrid}" \
  MIMO_USE_EXT_EQ2_INVCOEFF="${ext_eq2_invcoeff}" \
  MIMO_USE_EXT_EQ2_M2_BATCH="${ext_eq2_m2}" \
  MIMO_USE_EXT_EQ2_RHS_INTERLEAVE="${ext_eq2_rhs_interleave}" \
  MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13="${ext_eq2_mmse_adaptive_q13}" \
  MIMO_USE_SOLVE_STORE_TRUST_I16=$([[ "${ext_eq2_mmse_adaptive_q13}" == "1" ]] && echo 0 || echo 1) \
  MIMO_EMU_REQUIRE_NO_HARD_DIV=1 \
  bash scripts/submit_emu_kernel_once.sh ext "${KERNEL}" 0
