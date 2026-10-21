#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
APP=${ROOT}/nexus-am/apps/mimo
EMU_ENV="${ROOT:-${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}}"
TIMEOUT_SEC=${TIMEOUT_SEC:-900}
KERNEL=${1:-eq_zf_2x2}
TAG=${2:-eq2-rvv-perf-smoke}

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
rvv_eq2_m1=${MIMO_USE_RVV_EQ2_M1:-1}
eq2_zf_direct_h=${MIMO_USE_EQ2_ZF_DIRECT_H:-0}
eq2_zf_guard1_fine_recip=${MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP:-0}
rvv_eq2_split_build=${MIMO_USE_RVV_EQ2_SPLIT_BUILD:-0}
rvv_eq2_fused_nclip=${MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD:-1}
rvv_eq2_tx_soa=${MIMO_USE_RVV_EQ2_TX_SOA_STORE:-1}
rvv_eq2_3m=${MIMO_USE_RVV_EQ2_3M_BUILD:-0}
rvv_eq2_zext=${MIMO_USE_RVV_EQ2_ZEXT_PACK:-1}
rvv_eq2_ri_soa_output=${MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT:-0}
rvv_eq2_staged_narrow=${MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD:-0}
rvv_eq2_implicit_vxrm=${MIMO_USE_RVV_EQ2_IMPLICIT_VXRM:-0}
rvv_eq2_uq12_scale=${MIMO_USE_RVV_EQ2_UQ12_SCALE:-0}
rvv_eq2_early_recip_overlap=${MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP:-0}
rvv_eq2_mmse_full_trunc_scale=${MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE:-0}
rvv_eq2_block_soa=${MIMO_USE_RVV_EQ2_BLOCK_SOA:-0}
rvv_eq2_group_load=${MIMO_USE_RVV_EQ2_GROUP_LOAD:-0}
rvv_eq2_group_load_m4=${MIMO_USE_RVV_EQ2_GROUP_LOAD_M4:-0}
if [[ "${rvv_eq2_implicit_vxrm}" != "0" && "${rvv_eq2_implicit_vxrm}" != "1" ]]; then
  echo "MIMO_USE_RVV_EQ2_IMPLICIT_VXRM must be 0 or 1." >&2
  exit 2
fi
if [[ "${rvv_eq2_split_build}" != "0" && "${rvv_eq2_split_build}" != "1" ]]; then
  echo "MIMO_USE_RVV_EQ2_SPLIT_BUILD must be 0 or 1." >&2
  exit 2
fi
if [[ "${rvv_eq2_split_build}" == "1" && "${rvv_eq2_staged_narrow}" == "1" ]]; then
  echo "Split build and staged narrowing are mutually exclusive." >&2
  exit 2
fi
if [[ "${eq2_zf_guard1_fine_recip}" != "0" && "${eq2_zf_guard1_fine_recip}" != "1" ]]; then
  echo "MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP must be 0 or 1." >&2
  exit 2
fi
if [[ "${eq2_zf_guard1_fine_recip}" == "1" &&
      ( "${KERNEL}" != "eq_zf_2x2" || "${eq2_zf_direct_h}" != "1" ||
        "${rvv_eq2_m1}" != "1" || "${rvv_eq2_implicit_vxrm}" != "1" ||
        "${rvv_eq2_uq12_scale}" != "0" ) ]]; then
  echo "Guard1 fine reciprocal requires ZF2 direct-H, wide-m1, implicit RNU, and Q30 scale." >&2
  exit 2
fi
if [[ "${rvv_eq2_uq12_scale}" != "0" && "${rvv_eq2_uq12_scale}" != "1" ]]; then
  echo "MIMO_USE_RVV_EQ2_UQ12_SCALE must be 0 or 1." >&2
  exit 2
fi
if [[ "${rvv_eq2_early_recip_overlap}" != "0" && "${rvv_eq2_early_recip_overlap}" != "1" ]]; then
  echo "MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP must be 0 or 1." >&2
  exit 2
fi
if [[ "${rvv_eq2_early_recip_overlap}" == "1" &&
      ( "${KERNEL}" != "eq_mmse_2x2" || "${rvv_eq2_m1}" != "1" ) ]]; then
  echo "Early reciprocal overlap requires MMSE2 wide RVV mode." >&2
  exit 2
fi
if [[ "${rvv_eq2_early_recip_overlap}" == "1" && "${rvv_eq2_uq12_scale}" == "1" ]]; then
  echo "Early reciprocal overlap and UQ12 scale are mutually exclusive." >&2
  exit 2
fi
if [[ "${rvv_eq2_mmse_full_trunc_scale}" != "0" &&
      "${rvv_eq2_mmse_full_trunc_scale}" != "1" ]]; then
  echo "MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE must be 0 or 1." >&2
  exit 2
fi
if [[ "${rvv_eq2_mmse_full_trunc_scale}" == "1" &&
      ( "${KERNEL}" != "eq_mmse_2x2" || "${rvv_eq2_m1}" != "1" ||
        "${rvv_eq2_uq12_scale}" != "0" ) ]]; then
  echo "RVV full trunc scale requires MMSE2 wide-m1 and the Q30 scale path." >&2
  exit 2
fi
if [[ "${rvv_eq2_implicit_vxrm}" == "1" &&
      "${eq2_zf_direct_h}" != "1" &&
      ( "${KERNEL}" != "eq_mmse_2x2" ||
        "${rvv_eq2_staged_narrow}" != "1" ||
        "${rvv_eq2_ri_soa_output}" != "1" ) ]]; then
  echo "Implicit-vxrm requires ZF direct-H or MMSE2 staged RI-SoA." >&2
  exit 2
fi
if [[ "${rvv_eq2_block_soa}" == "1" && "${rvv_eq2_m1}" != "1" ]]; then
  echo "Block-SoA requires the wide RVV mode." >&2
  exit 2
fi
if [[ "${rvv_eq2_group_load}" == "1" && "${rvv_eq2_block_soa}" != "1" ]]; then
  echo "Group load requires block-SoA." >&2
  exit 2
fi
if [[ "${rvv_eq2_group_load_m4}" == "1" && "${rvv_eq2_group_load}" != "1" ]]; then
  echo "m4 group load requires group load." >&2
  exit 2
fi
if [[ "${eq2_zf_direct_h}" == "1" ]]; then
  rvv_eq2_fused_nclip=0
  rvv_eq2_tx_soa=0
  rvv_eq2_3m=0
  rvv_eq2_zext=0
  rvv_eq2_ri_soa_output=1
  rvv_eq2_staged_narrow=0
fi
if [[ "${rvv_eq2_ri_soa_output}" == "1" ]]; then
  rvv_eq2_tx_soa=0
  rvv_eq2_zext=0
fi

cd "${ROOT}"
source ./env.sh >/dev/null
cd "${APP}"

mkdir -p perf_eval/raw/nemu
log="${APP}/perf_eval/raw/nemu/nemu-mimo-rvv-${KERNEL}-perf-smoke-${TAG}.log"
am_log="${APP}/perf_eval/raw/nemu/nemu-mimo-rvv-${KERNEL}-perf-smoke-${TAG}-am-log.txt"

echo "MIMO_EQ2_RVV_PERF_SMOKE_START kernel=${KERNEL} tag=${TAG} timeout_sec=${TIMEOUT_SEC} direct_h=${eq2_zf_direct_h} guard1_fine_recip=${eq2_zf_guard1_fine_recip} m1=${rvv_eq2_m1} split_build=${rvv_eq2_split_build} fused_nclip=${rvv_eq2_fused_nclip} tx_soa=${rvv_eq2_tx_soa} build_3m=${rvv_eq2_3m} zext=${rvv_eq2_zext} ri_soa_output=${rvv_eq2_ri_soa_output} staged_narrow=${rvv_eq2_staged_narrow} implicit_vxrm=${rvv_eq2_implicit_vxrm} uq12_scale=${rvv_eq2_uq12_scale} early_recip_overlap=${rvv_eq2_early_recip_overlap} mmse_full_trunc_scale=${rvv_eq2_mmse_full_trunc_scale} block_soa=${rvv_eq2_block_soa} group_load=${rvv_eq2_group_load} group_load_m4=${rvv_eq2_group_load_m4}"

make ARCH=riscv64-nemu MIMO_IMPL=rvv clean >/dev/null
rm -f build/nemu-log.txt

set +e
timeout "${TIMEOUT_SEC}" \
  make ARCH=riscv64-nemu MIMO_IMPL=rvv \
    MIMO_PERF=1 \
    MIMO_VALIDATE=0 \
    MIMO_PERF_KERNEL="${KERNEL}" \
    MIMO_WARMUP_REPEAT="${warmup}" \
    MIMO_REPEAT="${repeat}" \
    MIMO_EQ2_RECIP_MODE=5 \
    MIMO_EQ2_RECIP_NUM_SHIFT=13 \
    MIMO_USE_EQ2_ZF_DIRECT_H="${eq2_zf_direct_h}" \
    MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP="${eq2_zf_guard1_fine_recip}" \
    MIMO_USE_RVV_EQ2_M1="${rvv_eq2_m1}" \
    MIMO_USE_RVV_EQ2_ALWAYS_ADD_LAMBDA=0 \
    MIMO_USE_RVV_EQ2_SPLIT_BUILD="${rvv_eq2_split_build}" \
    MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD="${rvv_eq2_fused_nclip}" \
    MIMO_USE_RVV_EQ2_TX_SOA_STORE="${rvv_eq2_tx_soa}" \
    MIMO_USE_RVV_EQ2_3M_BUILD="${rvv_eq2_3m}" \
    MIMO_USE_RVV_EQ2_ZEXT_PACK="${rvv_eq2_zext}" \
    MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT="${rvv_eq2_ri_soa_output}" \
    MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD="${rvv_eq2_staged_narrow}" \
    MIMO_USE_RVV_EQ2_IMPLICIT_VXRM="${rvv_eq2_implicit_vxrm}" \
    MIMO_USE_RVV_EQ2_UQ12_SCALE="${rvv_eq2_uq12_scale}" \
    MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP="${rvv_eq2_early_recip_overlap}" \
    MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE="${rvv_eq2_mmse_full_trunc_scale}" \
    MIMO_USE_RVV_EQ2_BLOCK_SOA="${rvv_eq2_block_soa}" \
    MIMO_USE_RVV_EQ2_GROUP_LOAD="${rvv_eq2_group_load}" \
    MIMO_USE_RVV_EQ2_GROUP_LOAD_M4="${rvv_eq2_group_load_m4}" \
    run 2>&1 | tee "${log}"
status=${PIPESTATUS[0]}
set -e

if [[ -f build/nemu-log.txt ]]; then
  cp build/nemu-log.txt "${am_log}"
fi

grep -E 'MIMO_ROI|MIMO_DONE|HIT GOOD TRAP|BAD TRAP' "${log}" || true

if [[ "${status}" -ne 0 ]] ||
   ! grep -q "MIMO_ROI kernel=${KERNEL} backend=rvv" "${log}" ||
   ! grep -q 'MIMO_DONE status=0' "${log}"; then
  echo "MIMO_EQ2_RVV_PERF_SMOKE_FAILED kernel=${KERNEL} tag=${TAG} status=${status} log=${log}" >&2
  exit 1
fi

echo "MIMO_EQ2_RVV_PERF_SMOKE_DONE kernel=${KERNEL} tag=${TAG} log=${log} am_log=${am_log}"
