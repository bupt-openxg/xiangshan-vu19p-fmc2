#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
FINAL_ROOT="${FINAL_ROOT:-${APP_DIR}/perf_eval}"
STAGE_ROOT="${STAGE_ROOT:-$(mktemp -d /tmp/fft-perf-refresh-all.XXXXXX)}"
BASELINE_STAGE_ROOT="${BASELINE_STAGE_ROOT:-${STAGE_ROOT}/baseline}"
EXT_STAGE_ROOT="${EXT_STAGE_ROOT:-${STAGE_ROOT}/ext}"
JOBS="${JOBS:-8}"
KEEP_BUILD="${KEEP_BUILD:-1}"
RUN_CANONICAL_SWEEP="${RUN_CANONICAL_SWEEP:-1}"
RUN_EXT_VALIDATE="${RUN_EXT_VALIDATE:-0}"
RUN_EXT_SWEEP="${RUN_EXT_SWEEP:-1}"
PUBLISH_TO_FINAL_ROOT="${PUBLISH_TO_FINAL_ROOT:-1}"
GENERATE_EXT_REPORTS="${GENERATE_EXT_REPORTS:-1}"

log_step() {
  printf '\n=== %s ===\n' "$*"
}

log_step "Refresh scalar + rvv into ${FINAL_ROOT}"
FINAL_ROOT="${FINAL_ROOT}" \
STAGE_ROOT="${BASELINE_STAGE_ROOT}" \
JOBS="${JOBS}" \
KEEP_BUILD="${KEEP_BUILD}" \
RUN_CANONICAL_SWEEP="${RUN_CANONICAL_SWEEP}" \
bash "${SCRIPT_DIR}/refresh_perf_eval.sh"

log_step "Run ext mainline sweep and refresh ext reports"
FINAL_ROOT="${FINAL_ROOT}" \
STAGE_ROOT="${EXT_STAGE_ROOT}" \
JOBS="${JOBS}" \
KEEP_BUILD="${KEEP_BUILD}" \
RUN_EXT_VALIDATE="${RUN_EXT_VALIDATE}" \
RUN_EXT_SWEEP="${RUN_EXT_SWEEP}" \
PUBLISH_TO_FINAL_ROOT="${PUBLISH_TO_FINAL_ROOT}" \
GENERATE_REPORTS="${GENERATE_EXT_REPORTS}" \
bash "${SCRIPT_DIR}/run_ext_mainline_canonical_sweep.sh"

log_step "All scalar / rvv / ext refresh work complete"
printf 'Published root: %s\n' "${FINAL_ROOT}"
printf 'Wrapper stage root: %s\n' "${STAGE_ROOT}"
printf 'Baseline stage root: %s\n' "${BASELINE_STAGE_ROOT}"
printf 'Ext stage root: %s\n' "${EXT_STAGE_ROOT}"
