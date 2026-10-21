#!/usr/bin/env bash
set -euo pipefail

APP="${MIMO_APP_DIR:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)}"
TIMEOUT_SEC=${TIMEOUT_SEC:-900}

cd "${APP}"

TIMEOUT_SEC="${TIMEOUT_SEC}" bash scripts/run_zf4_scalar_tol11_gate.sh eq4-final-zf4-scalar
TIMEOUT_SEC="${TIMEOUT_SEC}" bash scripts/run_zf4_rvv_current_best_gate.sh eq4-final-zf4-rvv
TIMEOUT_SEC="${TIMEOUT_SEC}" bash scripts/run_zf4_ext_packed_current_best_gate.sh eq4-final-zf4-ext
TIMEOUT_SEC="${TIMEOUT_SEC}" bash scripts/run_mmse4_scalar_current_best_gate.sh eq4-final-mmse4-scalar
TIMEOUT_SEC="${TIMEOUT_SEC}" bash scripts/run_mmse4_rvv_current_best_gate.sh eq4-final-mmse4-rvv
TIMEOUT_SEC="${TIMEOUT_SEC}" bash scripts/run_mmse4_ext_current_best_gate.sh eq4-final-mmse4-ext

echo "MIMO_EQ4_FINAL_CURRENT_BEST_GATES_PASS"
