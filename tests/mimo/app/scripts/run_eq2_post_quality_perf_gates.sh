#!/usr/bin/env bash
set -euo pipefail

APP="${MIMO_APP_DIR:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)}"
TIMEOUT_SEC=${TIMEOUT_SEC:-900}
TAG=${1:-post-quality-v1}

if ! [[ "${TAG}" =~ ^[A-Za-z0-9._-]+$ ]]; then
  echo "Invalid tag: ${TAG}" >&2
  exit 2
fi

cd "${APP}"

TIMEOUT_SEC="${TIMEOUT_SEC}" \
MIMO_USE_EQ2_ZF_DIRECT_H=1 \
MIMO_USE_RVV_EQ2_M1=1 \
MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT=1 \
MIMO_USE_RVV_EQ2_IMPLICIT_VXRM=1 \
  bash scripts/run_eq2_rvv_perf_smoke.sh \
    eq_zf_2x2 "${TAG}-rvv-zf"

TIMEOUT_SEC="${TIMEOUT_SEC}" \
MIMO_USE_EQ2_ZF_DIRECT_H=0 \
MIMO_USE_RVV_EQ2_M1=1 \
MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT=1 \
MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD=0 \
MIMO_USE_RVV_EQ2_IMPLICIT_VXRM=0 \
  bash scripts/run_eq2_rvv_perf_smoke.sh \
    eq_mmse_2x2 "${TAG}-rvv-mmse"

TIMEOUT_SEC="${TIMEOUT_SEC}" \
MIMO_USE_EQ2_ZF_DIRECT_H=1 \
MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID=1 \
MIMO_USE_EXT_EQ2_M2_BATCH=1 \
MIMO_USE_EXT_EQ2_RHS_INTERLEAVE=0 \
MIMO_USE_SOLVE_STORE_TRUST_I16=1 \
  bash scripts/run_eq2_ext_perf_smoke.sh \
    eq_zf_2x2 "${TAG}-ext-zf"

TIMEOUT_SEC="${TIMEOUT_SEC}" \
MIMO_USE_EQ2_ZF_DIRECT_H=0 \
MIMO_USE_EXT_EQ2_INVCOEFF=1 \
MIMO_USE_EXT_EQ2_M2_BATCH=1 \
MIMO_USE_EXT_EQ2_RHS_INTERLEAVE=0 \
MIMO_USE_SOLVE_STORE_TRUST_I16=1 \
  bash scripts/run_eq2_ext_perf_smoke.sh \
    eq_mmse_2x2 "${TAG}-ext-mmse"

echo "MIMO_EQ2_POST_QUALITY_PERF_GATES_DONE tag=${TAG}"
