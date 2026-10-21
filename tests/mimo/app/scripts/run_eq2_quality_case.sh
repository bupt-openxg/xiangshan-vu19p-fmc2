#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
APP=${ROOT}/nexus-am/apps/mimo
EMU_ENV="${ROOT:-${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}}"
TIMEOUT_SEC=${TIMEOUT_SEC:-900}
FAIL_PRINT_LIMIT=${MIMO_VALIDATE_FAIL_PRINT_LIMIT:-0}
QUALITY_TOL=${MIMO_EQ2_QUALITY_TOL:-11}
EXPERIMENTAL_PROFILE=${MIMO_EQ2_EXPERIMENTAL_PROFILE:-0}
EQ2_ZF_DIRECT_H=${MIMO_USE_EQ2_ZF_DIRECT_H:-1}
EQ2_MMSE_DIRECT_H=${MIMO_USE_EQ2_MMSE_DIRECT_H:-0}
EQ2_ZF_GUARD1_FINE_RECIP=${MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP:-0}
RVV_EQ2_UQ12_SCALE=${MIMO_USE_RVV_EQ2_UQ12_SCALE:-0}
RVV_EQ2_EARLY_RECIP_OVERLAP=${MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP:-0}
RVV_EQ2_MMSE_FULL_TRUNC_SCALE=${MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE:-0}
RVV_EQ2_STAGED_NARROW_BUILD=${MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD:-0}
RVV_EQ2_BLOCK_SOA=${MIMO_USE_RVV_EQ2_BLOCK_SOA:-0}
RVV_EQ2_GROUP_LOAD=${MIMO_USE_RVV_EQ2_GROUP_LOAD:-0}
RVV_EQ2_GROUP_LOAD_M4=${MIMO_USE_RVV_EQ2_GROUP_LOAD_M4:-0}
SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE=${MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE:-0}
SOLVE_STORE_TRUST_I16=${MIMO_USE_SOLVE_STORE_TRUST_I16:-0}
EXT_EQ2_INVCOEFF=${MIMO_USE_EXT_EQ2_INVCOEFF:-}
EXT_EQ2_ZF_GUARD1_HYBRID=${MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID:-0}
EXT_EQ2_M2_BATCH=${MIMO_USE_EXT_EQ2_M2_BATCH:-0}
EXT_EQ2_MMSE_ADAPTIVE_Q13=${MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13:-0}

backend=${1:-}
kernel=${2:-}
mod_bits=${3:-}
noise_q15=${4:-}
cond_profile=${5:-}
tag=${6:-quality}

case "${backend}" in scalar|rvv|ext) ;; *) echo "Unsupported backend: ${backend}" >&2; exit 2 ;; esac
case "${kernel}" in eq_zf_2x2|eq_mmse_2x2) ;; *) echo "Unsupported kernel: ${kernel}" >&2; exit 2 ;; esac
case "${mod_bits}" in 2|4|6) ;; *) echo "mod_bits must be 2, 4, or 6" >&2; exit 2 ;; esac
case "${cond_profile}" in 0|1|2) ;; *) echo "cond_profile must be 0, 1, or 2" >&2; exit 2 ;; esac
if ! [[ "${noise_q15}" =~ ^[0-9]+$ ]]; then
  echo "noise_q15 must be a non-negative integer" >&2
  exit 2
fi
if [[ "${QUALITY_TOL}" != "11" ]]; then
  echo "MIMO_EQ2_QUALITY_TOL is fixed at 11; 12 LSB or more must fail" >&2
  exit 2
fi
if [[ "${EXPERIMENTAL_PROFILE}" != "0" && "${EXPERIMENTAL_PROFILE}" != "1" ]]; then
  echo "MIMO_EQ2_EXPERIMENTAL_PROFILE must be 0 or 1" >&2
  exit 2
fi
if [[ "${EQ2_ZF_DIRECT_H}" != "0" && "${EQ2_ZF_DIRECT_H}" != "1" ]]; then
  echo "MIMO_USE_EQ2_ZF_DIRECT_H must be 0 or 1" >&2
  exit 2
fi
if [[ "${EQ2_MMSE_DIRECT_H}" != "0" && "${EQ2_MMSE_DIRECT_H}" != "1" ]]; then
  echo "MIMO_USE_EQ2_MMSE_DIRECT_H must be 0 or 1" >&2
  exit 2
fi
if [[ "${EQ2_MMSE_DIRECT_H}" == "1" && "${kernel}" != "eq_mmse_2x2" ]]; then
  echo "MIMO_USE_EQ2_MMSE_DIRECT_H is valid only for eq_mmse_2x2" >&2
  exit 2
fi
if [[ "${kernel}" == "eq_mmse_2x2" ]]; then
  EQ2_ZF_DIRECT_H=0
fi
if [[ -z "${EXT_EQ2_INVCOEFF}" ]]; then
  if [[ "${backend}" == "ext" && "${kernel}" == "eq_mmse_2x2" ]]; then
    EXT_EQ2_INVCOEFF=1
  else
    EXT_EQ2_INVCOEFF=0
  fi
fi
if [[ "${EXT_EQ2_INVCOEFF}" != "0" && "${EXT_EQ2_INVCOEFF}" != "1" ]]; then
  echo "MIMO_USE_EXT_EQ2_INVCOEFF must be 0 or 1" >&2
  exit 2
fi
if [[ "${backend}" == "ext" && "${kernel}" == "eq_zf_2x2" &&
      "${EQ2_ZF_DIRECT_H}" == "1" && "${EXT_EQ2_INVCOEFF}" != "0" ]]; then
  echo "EXT ZF2 direct-H requires MIMO_USE_EXT_EQ2_INVCOEFF=0" >&2
  exit 2
fi
if [[ "${EXT_EQ2_ZF_GUARD1_HYBRID}" != "0" &&
      "${EXT_EQ2_ZF_GUARD1_HYBRID}" != "1" ]]; then
  echo "MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID must be 0 or 1" >&2
  exit 2
fi
if [[ "${EXT_EQ2_ZF_GUARD1_HYBRID}" == "1" &&
      ( "${backend}" != "ext" || "${kernel}" != "eq_zf_2x2" ||
        "${EQ2_ZF_DIRECT_H}" != "1" || "${EXT_EQ2_INVCOEFF}" != "0" ) ]]; then
  echo "EXT guard1 hybrid requires EXT ZF2 direct-H and invcoeff=0" >&2
  exit 2
fi
if [[ "${EXT_EQ2_MMSE_ADAPTIVE_Q13}" != "0" &&
      "${EXT_EQ2_MMSE_ADAPTIVE_Q13}" != "1" ]]; then
  echo "MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13 must be 0 or 1" >&2
  exit 2
fi
if [[ "${EXT_EQ2_MMSE_ADAPTIVE_Q13}" == "1" &&
      ( "${backend}" != "ext" || "${kernel}" != "eq_mmse_2x2" ||
        "${EXT_EQ2_INVCOEFF}" != "1" ) ]]; then
  echo "EXT adaptive Q13 requires EXT MMSE2 with invcoeff=1" >&2
  exit 2
fi
if [[ "${EQ2_ZF_GUARD1_FINE_RECIP}" != "0" &&
      "${EQ2_ZF_GUARD1_FINE_RECIP}" != "1" ]]; then
  echo "MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP must be 0 or 1" >&2
  exit 2
fi
if [[ "${EQ2_ZF_GUARD1_FINE_RECIP}" == "1" &&
      ( "${kernel}" != "eq_zf_2x2" || "${EQ2_ZF_DIRECT_H}" != "1" ||
        "${backend}" == "ext" ) ]]; then
  echo "Guard1 fine reciprocal is valid only for scalar/RVV ZF2 direct-H" >&2
  exit 2
fi
if [[ "${EQ2_ZF_GUARD1_FINE_RECIP}" == "1" && "${backend}" == "rvv" &&
      "${RVV_EQ2_UQ12_SCALE}" != "0" ]]; then
  echo "RVV guard1 requires the Q30 scale path" >&2
  exit 2
fi
if [[ "${RVV_EQ2_EARLY_RECIP_OVERLAP}" != "0" && "${RVV_EQ2_EARLY_RECIP_OVERLAP}" != "1" ]]; then
  echo "MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP must be 0 or 1" >&2
  exit 2
fi
if [[ "${RVV_EQ2_EARLY_RECIP_OVERLAP}" == "1" &&
      ( "${backend}" != "rvv" || "${kernel}" != "eq_mmse_2x2" ) ]]; then
  echo "Early reciprocal overlap is valid only for RVV MMSE2" >&2
  exit 2
fi
if [[ "${RVV_EQ2_UQ12_SCALE}" == "1" && "${RVV_EQ2_EARLY_RECIP_OVERLAP}" == "1" ]]; then
  echo "MIMO_USE_RVV_EQ2_UQ12_SCALE and early reciprocal overlap are mutually exclusive" >&2
  exit 2
fi
if [[ "${RVV_EQ2_MMSE_FULL_TRUNC_SCALE}" != "0" &&
      "${RVV_EQ2_MMSE_FULL_TRUNC_SCALE}" != "1" ]]; then
  echo "MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE must be 0 or 1" >&2
  exit 2
fi
if [[ "${RVV_EQ2_MMSE_FULL_TRUNC_SCALE}" == "1" &&
      ( "${backend}" != "rvv" || "${kernel}" != "eq_mmse_2x2" ||
        "${RVV_EQ2_UQ12_SCALE}" != "0" ) ]]; then
  echo "RVV full trunc scale requires RVV MMSE2 and the Q30 scale path" >&2
  exit 2
fi
if [[ "${RVV_EQ2_STAGED_NARROW_BUILD}" != "0" &&
      "${RVV_EQ2_STAGED_NARROW_BUILD}" != "1" ]]; then
  echo "MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD must be 0 or 1" >&2
  exit 2
fi
for value in "${RVV_EQ2_BLOCK_SOA}" "${RVV_EQ2_GROUP_LOAD}" \
             "${RVV_EQ2_GROUP_LOAD_M4}"; do
  if [[ "${value}" != "0" && "${value}" != "1" ]]; then
    echo "RVV block/group-load controls must be 0 or 1" >&2
    exit 2
  fi
done
if [[ "${RVV_EQ2_BLOCK_SOA}" == "1" && "${backend}" != "rvv" ]]; then
  echo "Block-SoA quality evaluation is valid only for RVV" >&2
  exit 2
fi
if [[ "${RVV_EQ2_GROUP_LOAD}" == "1" && "${RVV_EQ2_BLOCK_SOA}" != "1" ]]; then
  echo "RVV group load requires block-SoA" >&2
  exit 2
fi
if [[ "${RVV_EQ2_GROUP_LOAD_M4}" == "1" && "${RVV_EQ2_GROUP_LOAD}" != "1" ]]; then
  echo "RVV m4 group load requires group load" >&2
  exit 2
fi
if [[ "${SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE}" != "0" &&
      "${SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE}" != "1" ]]; then
  echo "MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE must be 0 or 1" >&2
  exit 2
fi
if [[ "${SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE}" == "1" &&
      ( "${backend}" != "scalar" || "${kernel}" != "eq_mmse_2x2" ) ]]; then
  echo "Scalar full trunc scale is valid only for scalar MMSE2" >&2
  exit 2
fi
if [[ "${SOLVE_STORE_TRUST_I16}" != "0" &&
      "${SOLVE_STORE_TRUST_I16}" != "1" ]]; then
  echo "MIMO_USE_SOLVE_STORE_TRUST_I16 must be 0 or 1" >&2
  exit 2
fi
if ! [[ "${tag}" =~ ^[A-Za-z0-9._-]+$ ]]; then
  echo "Invalid tag: ${tag}" >&2
  exit 2
fi

if pgrep -f "${EMU_ENV}/XiangShan/build/emu" >/dev/null; then
  echo "Refusing EQ2 quality NEMU run while official emu is active." >&2
  exit 3
fi

cd "${ROOT}"
source ./env.sh >/dev/null
cd "${APP}"

mkdir -p perf_eval/raw/nemu/eq2-quality
log="${APP}/perf_eval/raw/nemu/eq2-quality/${tag}-${backend}-${kernel}-m${mod_bits}-n${noise_q15}-c${cond_profile}.log"
am_log="${log%.log}-am-log.txt"

make_args=(
  ARCH=riscv64-xs
  MIMO_IMPL="${backend}"
  MIMO_PERF=0
  MIMO_VALIDATE=1
  MIMO_VALIDATE_KERNEL="${kernel}"
  MIMO_VALIDATE_TOL="${QUALITY_TOL}"
  MIMO_VALIDATE_ERROR_STATS=1
  MIMO_VALIDATE_FAIL_PRINT_LIMIT="${FAIL_PRINT_LIMIT}"
  MIMO_EQ2_QUALITY_EVAL=1
  MIMO_EQ2_QUALITY_MOD_BITS="${mod_bits}"
  MIMO_EQ2_QUALITY_NOISE_Q15="${noise_q15}"
  MIMO_EQ2_QUALITY_COND_PROFILE="${cond_profile}"
  MIMO_EQ2_EXPERIMENTAL_PROFILE="${EXPERIMENTAL_PROFILE}"
  MIMO_EQ2_RECIP_MODE=5
  MIMO_EQ2_RECIP_NUM_SHIFT=13
  MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE="${SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE}"
  MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE="${RVV_EQ2_MMSE_FULL_TRUNC_SCALE}"
  MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP="${EQ2_ZF_GUARD1_FINE_RECIP}"
  MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID="${EXT_EQ2_ZF_GUARD1_HYBRID}"
  MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13="${EXT_EQ2_MMSE_ADAPTIVE_Q13}"
  MIMO_USE_EQ2_MMSE_DIRECT_H="${EQ2_MMSE_DIRECT_H}"
  MIMO_USE_SOLVE_STORE_TRUST_I16="${SOLVE_STORE_TRUST_I16}"
  MIMO_EMU_REQUIRE_NO_HARD_DIV=1
)

case "${backend}/${kernel}" in
  scalar/eq_zf_2x2)
    make_args+=(
      MIMO_USE_EQ2_ZF_DIRECT_H="${EQ2_ZF_DIRECT_H}"
      MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA=0
    )
    ;;
  scalar/eq_mmse_2x2)
    make_args+=(
      MIMO_USE_EQ2_ZF_DIRECT_H=0
      MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA=1
      MIMO_USE_SCALAR_MMSE4_FUSED_BUILD_SOLVE=1
      MIMO_USE_SOLVE4_SCHUR=1
      MIMO_USE_SOLVE4_SCHUR_Q15_RECIP=5
    )
    ;;
  rvv/eq_zf_2x2)
    make_args+=(
      MIMO_USE_EQ2_ZF_DIRECT_H="${EQ2_ZF_DIRECT_H}"
      MIMO_USE_RVV_EQ2_M1=1
      MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD=0
      MIMO_USE_RVV_EQ2_TX_SOA_STORE=0
      MIMO_USE_RVV_EQ2_ZEXT_PACK=0
      MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT=1
      MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD=0
      MIMO_USE_RVV_EQ2_IMPLICIT_VXRM=1
      MIMO_USE_RVV_EQ2_UQ12_SCALE="${RVV_EQ2_UQ12_SCALE}"
      MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP="${RVV_EQ2_EARLY_RECIP_OVERLAP}"
      MIMO_USE_RVV_EQ2_BLOCK_SOA="${RVV_EQ2_BLOCK_SOA}"
      MIMO_USE_RVV_EQ2_GROUP_LOAD="${RVV_EQ2_GROUP_LOAD}"
      MIMO_USE_RVV_EQ2_GROUP_LOAD_M4="${RVV_EQ2_GROUP_LOAD_M4}"
    )
    ;;
  rvv/eq_mmse_2x2)
    make_args+=(
      MIMO_USE_EQ2_ZF_DIRECT_H=0
      MIMO_USE_RVV_EQ2_M1=1
      MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD=1
      MIMO_USE_RVV_EQ2_TX_SOA_STORE=0
      MIMO_USE_RVV_EQ2_ZEXT_PACK=0
      MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT=1
      MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD="${RVV_EQ2_STAGED_NARROW_BUILD}"
      MIMO_USE_RVV_EQ2_IMPLICIT_VXRM=0
      MIMO_USE_RVV_EQ2_UQ12_SCALE="${RVV_EQ2_UQ12_SCALE}"
      MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP="${RVV_EQ2_EARLY_RECIP_OVERLAP}"
      MIMO_USE_RVV_EQ2_BLOCK_SOA="${RVV_EQ2_BLOCK_SOA}"
      MIMO_USE_RVV_EQ2_GROUP_LOAD="${RVV_EQ2_GROUP_LOAD}"
      MIMO_USE_RVV_EQ2_GROUP_LOAD_M4="${RVV_EQ2_GROUP_LOAD_M4}"
    )
    ;;
  ext/eq_zf_2x2)
    make_args+=(
      MIMO_USE_EQ2_ZF_DIRECT_H="${EQ2_ZF_DIRECT_H}"
      MIMO_USE_EXT_EQ2_INVCOEFF="${EXT_EQ2_INVCOEFF}"
      MIMO_USE_EXT_EQ2_M2_BATCH=1
      MIMO_USE_EXT_EQ2_RHS_INTERLEAVE=0
    )
    ;;
  ext/eq_mmse_2x2)
    if [[ "${EQ2_MMSE_DIRECT_H}" == "1" ]]; then
      EXT_EQ2_M2_BATCH=1
      EXT_EQ2_MMSE_ADAPTIVE_Q13=1
    fi
    make_args+=(
      MIMO_USE_EQ2_ZF_DIRECT_H=0
      MIMO_USE_EXT_EQ2_INVCOEFF="${EXT_EQ2_INVCOEFF}"
      MIMO_USE_EXT_EQ2_M2_BATCH="${EXT_EQ2_M2_BATCH}"
      MIMO_USE_EXT_EQ2_RHS_INTERLEAVE=0
      MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13="${EXT_EQ2_MMSE_ADAPTIVE_Q13}"
    )
    ;;
esac

echo "MIMO_EQ2_QUALITY_CASE_START backend=${backend} kernel=${kernel} mod_bits=${mod_bits} noise_q15=${noise_q15} cond_profile=${cond_profile} tol=${QUALITY_TOL} experimental_profile=${EXPERIMENTAL_PROFILE} zf_direct_h=${EQ2_ZF_DIRECT_H} mmse_direct_h=${EQ2_MMSE_DIRECT_H} guard1_fine_recip=${EQ2_ZF_GUARD1_FINE_RECIP} scalar_mmse_full_trunc_scale=${SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE} rvv_mmse_full_trunc_scale=${RVV_EQ2_MMSE_FULL_TRUNC_SCALE} store_trust_i16=${SOLVE_STORE_TRUST_I16} ext_invcoeff=${EXT_EQ2_INVCOEFF} ext_m2_batch=${EXT_EQ2_M2_BATCH} ext_adaptive_q13=${EXT_EQ2_MMSE_ADAPTIVE_Q13} ext_guard1_hybrid=${EXT_EQ2_ZF_GUARD1_HYBRID} early_recip_overlap=${RVV_EQ2_EARLY_RECIP_OVERLAP} staged_narrow=${RVV_EQ2_STAGED_NARROW_BUILD} block_soa=${RVV_EQ2_BLOCK_SOA} group_load=${RVV_EQ2_GROUP_LOAD} group_load_m4=${RVV_EQ2_GROUP_LOAD_M4}"
make ARCH=riscv64-xs MIMO_IMPL="${backend}" clean >/dev/null

set +e
make "${make_args[@]}" 2>&1 | tee "${log%.log}-build.log"
build_status=${PIPESTATUS[0]}
if [[ "${build_status}" -eq 0 ]]; then
  timeout "${TIMEOUT_SEC}" "${ROOT}/NEMU/build/riscv64-nemu-interpreter" \
    -b "${APP}/build/mimo-${backend}.bin" 2>&1 | tee "${log}"
  status=${PIPESTATUS[0]}
else
  status=${build_status}
fi
set -e

cp "${log}" "${am_log}"

if [[ "${status}" -ne 0 ]] ||
   ! grep -q "MIMO_VALIDATE_PASS kernel=${kernel}" "${log}" ||
   ! grep -q "MIMO_EQ2_QUALITY backend=${backend} kernel=${kernel}" "${log}" ||
   ! grep -q 'MIMO_DONE status=0' "${log}" ||
   ! grep -q 'HIT GOOD TRAP' "${am_log}"; then
  echo "MIMO_EQ2_QUALITY_CASE_FAILED status=${status} log=${log}" >&2
  exit 1
fi

quality_line=$(grep "MIMO_EQ2_QUALITY backend=${backend} kernel=${kernel}" "${log}" | tail -n 1)
det_min=$(sed -n 's/.* zf_det_q15_min=\([0-9][0-9]*\).*/\1/p' <<< "${quality_line}")
det_max=$(sed -n 's/.* zf_det_q15_max=\([0-9][0-9]*\).*/\1/p' <<< "${quality_line}")
if [[ -z "${det_min}" || -z "${det_max}" || "${det_min}" -lt 3400 || "${det_max}" -gt 4408 ]]; then
  echo "MIMO_EQ2_QUALITY_CASE_FAILED reciprocal_range=${det_min:-missing}..${det_max:-missing} log=${log}" >&2
  exit 1
fi

echo "${quality_line}"
echo "MIMO_EQ2_QUALITY_CASE_DONE log=${log} am_log=${am_log}"
