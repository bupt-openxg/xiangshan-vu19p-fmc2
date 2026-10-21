#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
APP=${ROOT}/nexus-am/apps/mimo
EMU_ENV="${ROOT:-${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}}"
TIMEOUT_SEC=${TIMEOUT_SEC:-900}
KERNEL=${1:-eq_zf_2x2}
TAG=${2:-eq2-ext-perf-smoke}

case "${KERNEL}" in
  eq_zf_2x2|eq_mmse_2x2) ;;
  *)
    echo "Unsupported EQ2 kernel: ${KERNEL}" >&2
    exit 2
    ;;
esac

if [[ ! "${TAG}" =~ ^[A-Za-z0-9._-]+$ ]]; then
  echo "Invalid tag '${TAG}'; use only letters, digits, dot, underscore, and dash." >&2
  exit 2
fi

if [[ "${MIMO_GATE_ALLOW_EMU_RUNNING:-0}" != "1" ]] &&
   pgrep -f "${EMU_ENV}/XiangShan/build/emu" >/dev/null; then
  echo "Refusing NEMU smoke while an official emu process is running." >&2
  exit 3
fi

warmup=${MIMO_WARMUP_REPEAT:-0}
repeat=${MIMO_REPEAT:-1}
experimental_profile=${MIMO_EQ2_EXPERIMENTAL_PROFILE:-0}
eq2_zf_direct_h=${MIMO_USE_EQ2_ZF_DIRECT_H:-0}
eq2_mmse_direct_h=${MIMO_USE_EQ2_MMSE_DIRECT_H:-0}
ext_eq2_zf_guard1_hybrid=${MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID:-0}
ext_eq2_invcoeff=${MIMO_USE_EXT_EQ2_INVCOEFF:-1}
ext_eq2_m2_batch=${MIMO_USE_EXT_EQ2_M2_BATCH:-0}
ext_eq2_rhs_interleave=${MIMO_USE_EXT_EQ2_RHS_INTERLEAVE:-0}
ext_eq2_mmse_adaptive_q13=${MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13:-0}
solve_store_trust_i16=${MIMO_USE_SOLVE_STORE_TRUST_I16:-}
if [[ "${experimental_profile}" != "0" && "${experimental_profile}" != "1" ]]; then
  echo "MIMO_EQ2_EXPERIMENTAL_PROFILE must be 0 or 1." >&2
  exit 2
fi
if [[ "${eq2_mmse_direct_h}" != "0" && "${eq2_mmse_direct_h}" != "1" ]]; then
  echo "MIMO_USE_EQ2_MMSE_DIRECT_H must be 0 or 1." >&2
  exit 2
fi
if [[ "${eq2_mmse_direct_h}" == "1" ]]; then
  if [[ "${KERNEL}" != "eq_mmse_2x2" ]]; then
    echo "MMSE direct identity is valid only for eq_mmse_2x2." >&2
    exit 2
  fi
  ext_eq2_m2_batch=1
  ext_eq2_mmse_adaptive_q13=1
  solve_store_trust_i16=0
fi
if [[ "${eq2_zf_direct_h}" == "1" ]]; then
  if [[ "${KERNEL}" != "eq_zf_2x2" ]]; then
    echo "EQ2 direct-H is valid only for eq_zf_2x2." >&2
    exit 2
  fi
  ext_eq2_invcoeff=0
  ext_eq2_m2_batch=1
  ext_eq2_rhs_interleave=0
fi
if [[ -z "${solve_store_trust_i16}" ]]; then
  solve_store_trust_i16=$([[ "${ext_eq2_mmse_adaptive_q13}" == "1" ]] && echo 0 || echo 1)
fi
if [[ "${ext_eq2_mmse_adaptive_q13}" != "0" &&
      "${ext_eq2_mmse_adaptive_q13}" != "1" ]]; then
  echo "MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13 must be 0 or 1." >&2
  exit 2
fi
if [[ "${ext_eq2_mmse_adaptive_q13}" == "1" &&
      ( "${KERNEL}" != "eq_mmse_2x2" || "${ext_eq2_invcoeff}" != "1" ||
        "${ext_eq2_rhs_interleave}" != "0" ||
        "${solve_store_trust_i16}" != "0" ) ]]; then
  echo "EXT adaptive Q13 requires MMSE2 invcoeff with saturated output." >&2
  exit 2
fi

if [[ "${ext_eq2_zf_guard1_hybrid}" != "0" &&
      "${ext_eq2_zf_guard1_hybrid}" != "1" ]]; then
  echo "MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID must be 0 or 1." >&2
  exit 2
fi
if [[ "${ext_eq2_zf_guard1_hybrid}" == "1" &&
      ( "${KERNEL}" != "eq_zf_2x2" || "${eq2_zf_direct_h}" != "1" ||
        "${ext_eq2_invcoeff}" != "0" || "${ext_eq2_m2_batch}" != "1" ||
        "${ext_eq2_rhs_interleave}" != "0" ) ]]; then
  echo "EXT ZF2 guard1 hybrid requires direct-H, m2, no invcoeff, and no RHS interleave." >&2
  exit 2
fi

cd "${ROOT}"
source ./env.sh >/dev/null
cd "${APP}"

mkdir -p perf_eval/raw/nemu
log="${APP}/perf_eval/raw/nemu/nemu-mimo-ext-${KERNEL}-perf-smoke-${TAG}.log"
am_log="${APP}/perf_eval/raw/nemu/nemu-mimo-ext-${KERNEL}-perf-smoke-${TAG}-am-log.txt"

echo "MIMO_EQ2_EXT_PERF_SMOKE_START kernel=${KERNEL} tag=${TAG} timeout_sec=${TIMEOUT_SEC} experimental_profile=${experimental_profile} zf_direct_h=${eq2_zf_direct_h} mmse_direct_h=${eq2_mmse_direct_h} guard1_hybrid=${ext_eq2_zf_guard1_hybrid} invcoeff=${ext_eq2_invcoeff} m2_batch=${ext_eq2_m2_batch} rhs_interleave=${ext_eq2_rhs_interleave} adaptive_q13=${ext_eq2_mmse_adaptive_q13}"

make ARCH=riscv64-nemu MIMO_IMPL=ext clean >/dev/null
rm -f build/nemu-log.txt

set +e
timeout "${TIMEOUT_SEC}" \
  make ARCH=riscv64-nemu MIMO_IMPL=ext \
    MIMO_PERF=1 \
    MIMO_VALIDATE=0 \
    MIMO_PERF_KERNEL="${KERNEL}" \
    MIMO_WARMUP_REPEAT="${warmup}" \
    MIMO_REPEAT="${repeat}" \
    MIMO_EQ2_EXPERIMENTAL_PROFILE="${experimental_profile}" \
    MIMO_EQ2_RECIP_MODE=5 \
    MIMO_EQ2_RECIP_NUM_SHIFT=13 \
    MIMO_USE_EQ2_ZF_DIRECT_H="${eq2_zf_direct_h}" \
    MIMO_USE_EQ2_MMSE_DIRECT_H="${eq2_mmse_direct_h}" \
    MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID="${ext_eq2_zf_guard1_hybrid}" \
    MIMO_USE_EXT_EQ2_INVCOEFF="${ext_eq2_invcoeff}" \
    MIMO_USE_EXT_EQ2_M2_BATCH="${ext_eq2_m2_batch}" \
    MIMO_USE_EXT_EQ2_RHS_INTERLEAVE="${ext_eq2_rhs_interleave}" \
    MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13="${ext_eq2_mmse_adaptive_q13}" \
    MIMO_USE_SOLVE_STORE_TRUST_I16="${solve_store_trust_i16}" \
    run 2>&1 | tee "${log}"
status=${PIPESTATUS[0]}
set -e

if [[ -f build/nemu-log.txt ]]; then
  cp build/nemu-log.txt "${am_log}"
fi

grep -E 'MIMO_ROI|MIMO_DONE|HIT GOOD TRAP|BAD TRAP' "${log}" || true

if [[ "${status}" -ne 0 ]] ||
   ! grep -q "MIMO_ROI kernel=${KERNEL} backend=ext" "${log}" ||
   ! grep -q 'MIMO_DONE status=0' "${log}"; then
  echo "MIMO_EQ2_EXT_PERF_SMOKE_FAILED kernel=${KERNEL} tag=${TAG} status=${status} log=${log}" >&2
  exit 1
fi

echo "MIMO_EQ2_EXT_PERF_SMOKE_DONE kernel=${KERNEL} tag=${TAG} log=${log} am_log=${am_log}"
