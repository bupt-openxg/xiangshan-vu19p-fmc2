#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
APP=${ROOT}/nexus-am/apps/mimo
KERNEL=${1:-eq_zf_2x2}
MODE=${2:-wide}
TIMEOUT_SEC=${TIMEOUT_SEC:-0}
eq2_experimental_profile=${MIMO_EQ2_EXPERIMENTAL_PROFILE:-0}
eq2_zf_direct_h=${MIMO_USE_EQ2_ZF_DIRECT_H:-0}
eq2_zf_guard1_fine_recip=${MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP:-0}
rvv_eq2_ri_soa_output=${MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT:-0}
rvv_eq2_staged_narrow_build=${MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD:-0}
rvv_eq2_implicit_vxrm=${MIMO_USE_RVV_EQ2_IMPLICIT_VXRM:-0}
rvv_eq2_uq12_scale=${MIMO_USE_RVV_EQ2_UQ12_SCALE:-0}
rvv_eq2_early_recip_overlap=${MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP:-0}
rvv_eq2_mmse_full_trunc_scale=${MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE:-0}
rvv_eq2_block_soa=${MIMO_USE_RVV_EQ2_BLOCK_SOA:-0}
rvv_eq2_group_load=${MIMO_USE_RVV_EQ2_GROUP_LOAD:-0}
rvv_eq2_group_load_m4=${MIMO_USE_RVV_EQ2_GROUP_LOAD_M4:-0}
rvv_eq2_fused_nclip_build=1
rvv_eq2_tx_soa_store=1
rvv_eq2_zext_pack=1
if [[ "${rvv_eq2_ri_soa_output}" == "1" ]]; then
  rvv_eq2_tx_soa_store=0
  rvv_eq2_zext_pack=0
fi

case "${KERNEL}" in
  eq_zf_2x2|eq_mmse_2x2) ;;
  *) echo "Unsupported EQ2 RVV kernel: ${KERNEL}" >&2; exit 2 ;;
esac
case "${MODE}" in
  wide) rvv_eq2_m1=1 ;;
  narrow) rvv_eq2_m1=0 ;;
  *) echo "Unsupported EQ2 RVV mode: ${MODE}; use wide or narrow." >&2; exit 2 ;;
esac

if [[ "${eq2_zf_direct_h}" == "1" ]]; then
  if [[ "${KERNEL}" != "eq_zf_2x2" ]]; then
    echo "EQ2 direct-H is valid only for eq_zf_2x2." >&2
    exit 2
  fi
  rvv_eq2_fused_nclip_build=0
  rvv_eq2_tx_soa_store=0
  rvv_eq2_zext_pack=0
  rvv_eq2_ri_soa_output=1
  rvv_eq2_staged_narrow_build=0
fi

if [[ "${rvv_eq2_implicit_vxrm}" != "0" && "${rvv_eq2_implicit_vxrm}" != "1" ]]; then
  echo "MIMO_USE_RVV_EQ2_IMPLICIT_VXRM must be 0 or 1." >&2
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
  echo "RVV guard1 fine reciprocal requires ZF2 wide direct-H, implicit RNU, and Q30 scale." >&2
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
if [[ "${eq2_experimental_profile}" != "0" &&
      "${eq2_experimental_profile}" != "1" ]]; then
  echo "MIMO_EQ2_EXPERIMENTAL_PROFILE must be 0 or 1." >&2
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
        "${rvv_eq2_staged_narrow_build}" != "1" ||
        "${rvv_eq2_ri_soa_output}" != "1" ) ]]; then
  echo "Implicit-vxrm requires ZF direct-H or MMSE2 staged RI-SoA." >&2
  exit 2
fi
if [[ "${rvv_eq2_block_soa}" == "1" && "${rvv_eq2_m1}" != "1" ]]; then
  echo "Block-SoA requires wide mode." >&2
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

if [[ "${TIMEOUT_SEC}" != "0" ]]; then
  echo "Refusing EQ2 RVV official: TIMEOUT_SEC must be 0." >&2
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
  MIMO_EQ2_EXPERIMENTAL_PROFILE="${eq2_experimental_profile}" \
  MIMO_EQ2_RECIP_MODE=5 \
  MIMO_EQ2_RECIP_NUM_SHIFT=13 \
  MIMO_USE_EQ2_ZF_DIRECT_H="${eq2_zf_direct_h}" \
  MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP="${eq2_zf_guard1_fine_recip}" \
  MIMO_USE_RVV_EQ2_M1="${rvv_eq2_m1}" \
  MIMO_USE_RVV_EQ2_ALWAYS_ADD_LAMBDA=0 \
  MIMO_USE_RVV_EQ2_SPLIT_BUILD=0 \
  MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD="${rvv_eq2_fused_nclip_build}" \
  MIMO_USE_RVV_EQ2_TX_SOA_STORE="${rvv_eq2_tx_soa_store}" \
  MIMO_USE_RVV_EQ2_3M_BUILD=0 \
  MIMO_USE_RVV_EQ2_ZEXT_PACK="${rvv_eq2_zext_pack}" \
  MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT="${rvv_eq2_ri_soa_output}" \
  MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD="${rvv_eq2_staged_narrow_build}" \
  MIMO_USE_RVV_EQ2_IMPLICIT_VXRM="${rvv_eq2_implicit_vxrm}" \
  MIMO_USE_RVV_EQ2_UQ12_SCALE="${rvv_eq2_uq12_scale}" \
  MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP="${rvv_eq2_early_recip_overlap}" \
  MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE="${rvv_eq2_mmse_full_trunc_scale}" \
  MIMO_USE_RVV_EQ2_BLOCK_SOA="${rvv_eq2_block_soa}" \
  MIMO_USE_RVV_EQ2_GROUP_LOAD="${rvv_eq2_group_load}" \
  MIMO_USE_RVV_EQ2_GROUP_LOAD_M4="${rvv_eq2_group_load_m4}" \
  MIMO_USE_EXT_EQ2_INVCOEFF=1 \
  MIMO_USE_EXT_EQ2_M2_BATCH=0 \
  MIMO_EMU_REQUIRE_NO_HARD_DIV=1 \
  bash scripts/submit_emu_kernel_once.sh rvv "${KERNEL}" 0
