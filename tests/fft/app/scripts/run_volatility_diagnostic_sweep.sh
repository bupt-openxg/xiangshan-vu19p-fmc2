#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
TIMESTAMP="${TIMESTAMP:-$(date +%Y%m%dT%H%M%S)}"
STAGE_BASE="${STAGE_BASE:-/tmp/fft-volatility-diagnostic}"
STAGE_ROOT="${STAGE_ROOT:-${STAGE_BASE}/${TIMESTAMP}}"
JOBS="${JOBS:-8}"
KEEP_BUILD="${KEEP_BUILD:-1}"
WARMUP_REPEAT="${WARMUP_REPEAT:-2}"
HPM_PROFILE="${HPM_PROFILE:-diagnostic}"
IMPL_NAME_TAG="${IMPL_NAME_TAG:-diag-hpm}"
MATRIX_PATH="${STAGE_ROOT}/matrix.tsv"
STATUS_PATH="${STAGE_ROOT}/status.log"
README_PATH="${STAGE_ROOT}/README.txt"

mkdir -p "${STAGE_ROOT}"
mkdir -p "${STAGE_BASE}"
ln -sfn "${STAGE_ROOT}" "${STAGE_BASE}/latest"

cat > "${README_PATH}" <<EOF
FFT volatility diagnostic sweep
timestamp=${TIMESTAMP}
stage_root=${STAGE_ROOT}
warmup_repeat=${WARMUP_REPEAT}
hpm_profile=${HPM_PROFILE}
impl_name_tag=${IMPL_NAME_TAG}

Scope:
- representative high-volatility points only: N=64, 256, 1024
- compare low-repeat vs high-repeat under the same warmup policy
- run both scalar mainline and rvv mainline
- use diagnostic ROI HPM profile to attribute why scalar is more repeat-sensitive

Matrix:
- N=64  : repeats 10, 20
- N=256 : repeats 10, 20
- N=1024: repeats 3, 7
EOF

printf "version\timpl\tpoints\twarmup\tmeasured_repeat\thpm_profile\timpl_name\n" > "${MATRIX_PATH}"

log_status() {
  printf '[%s] %s\n' "$(date '+%F %T')" "$*" | tee -a "${STATUS_PATH}"
}

record_workload() {
  local version="$1"
  local impl="$2"
  local points="$3"
  local repeat="$4"
  printf "%s\t%s\t%s\t%s\t%s\t%s\t%s\n" \
    "${version}" "${impl}" "${points}" "${WARMUP_REPEAT}" "${repeat}" "${HPM_PROFILE}" "${IMPL_NAME_TAG}" \
    >> "${MATRIX_PATH}"
}

run_job() {
  local version="$1"
  local impl="$2"
  local points="$3"
  local repeat="$4"

  record_workload "${version}" "${impl}" "${points}" "${repeat}"
  log_status "START version=${version} impl=${impl} points=${points} warmup=${WARMUP_REPEAT} repeat=${repeat} hpm=${HPM_PROFILE}"

  DFT_IMPL="${impl}" \
  FFT_IMPL_NAME="${IMPL_NAME_TAG}" \
  FFT_VALIDATE=0 \
  FFT_POINTS="${points}" \
  FFT_REPEAT="${repeat}" \
  FFT_WARMUP_REPEAT="${WARMUP_REPEAT}" \
  FFT_RUN_IDFT=0 \
  FFT_ROI_HPM_PROFILE="${HPM_PROFILE}" \
  REPORT_ROOT="${STAGE_ROOT}" \
  JOBS="${JOBS}" \
  KEEP_BUILD="${KEEP_BUILD}" \
  "${SCRIPT_DIR}/run.sh"

  log_status "DONE version=${version} impl=${impl} points=${points} warmup=${WARMUP_REPEAT} repeat=${repeat} hpm=${HPM_PROFILE}"
}

log_status "Diagnostic sweep root prepared at ${STAGE_ROOT}"
log_status "Matrix will be recorded at ${MATRIX_PATH}"

for repeat in 10 20; do
  run_job scalar dft_scalar 64 "${repeat}"
done

for repeat in 10 20; do
  run_job scalar dft_scalar 256 "${repeat}"
done

for repeat in 3 7; do
  run_job scalar dft_scalar 1024 "${repeat}"
done

for repeat in 10 20; do
  run_job rvv dft_rvv 64 "${repeat}"
done

for repeat in 10 20; do
  run_job rvv dft_rvv 256 "${repeat}"
done

for repeat in 3 7; do
  run_job rvv dft_rvv 1024 "${repeat}"
done

log_status "Diagnostic sweep complete. Reports are under ${STAGE_ROOT}"
