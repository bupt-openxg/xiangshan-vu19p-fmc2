#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
TIMESTAMP="${TIMESTAMP:-$(date +%Y%m%dT%H%M%S)}"
STAGE_BASE="${STAGE_BASE:-/tmp/fft-canonical-baseline}"
STAGE_ROOT="${STAGE_ROOT:-${STAGE_BASE}/${TIMESTAMP}}"
JOBS="${JOBS:-8}"
KEEP_BUILD="${KEEP_BUILD:-1}"
MATRIX_PATH="${STAGE_ROOT}/matrix.tsv"
STATUS_PATH="${STAGE_ROOT}/status.log"
README_PATH="${STAGE_ROOT}/README.txt"

mkdir -p "${STAGE_ROOT}"
mkdir -p "${STAGE_BASE}"
ln -sfn "${STAGE_ROOT}" "${STAGE_BASE}/latest"

cat > "${README_PATH}" <<EOF
FFT canonical baseline sweep
timestamp=${TIMESTAMP}
stage_root=${STAGE_ROOT}

Policy:
- scalar baseline: DFT_IMPL=dft_scalar for every supported point 16-4096
- rvv baseline: DFT_IMPL=dft_rvv for every supported point 16-4096
- all perf runs use FFT_VALIDATE=0 and FFT_RUN_IDFT=0
- reports are staged under this root using the normal run.sh layout
- this sweep follows the current formal steady-state standard

Canonical matrix:
- N=16:   warmup 8, measured repeat 30, band 3.0%
- N=64:   warmup 8, measured repeat 10, band 3.0%
- N=128:  warmup 8, measured repeat 10, band 3.0%
- N=256:  warmup 6, measured repeat 10, band 1.0%
- N=512:  warmup 6, measured repeat 10, band 1.0%
- N=1024: warmup 6, measured repeat 5,  band 1.5%
- N=2048: warmup 4, measured repeat 3,  band 1.5%
- N=4096: warmup 4, measured repeat 3,  band 1.5%
EOF

printf "version\timpl\tpoints\twarmup\tmeasured_repeat\taccept_band_pct\n" > "${MATRIX_PATH}"

log_status() {
  printf '[%s] %s\n' "$(date '+%F %T')" "$*" | tee -a "${STATUS_PATH}"
}

accept_band_for_points() {
  local points="$1"
  if [[ "${points}" -le 128 ]]; then
    printf '3.0'
  elif [[ "${points}" -le 512 ]]; then
    printf '1.0'
  else
    printf '1.5'
  fi
}

record_workload() {
  local version="$1"
  local impl="$2"
  local points="$3"
  local warmup="$4"
  local repeat="$5"
  local accept_band
  accept_band="$(accept_band_for_points "${points}")"
  printf "%s\t%s\t%s\t%s\t%s\t%s\n" \
    "${version}" "${impl}" "${points}" "${warmup}" "${repeat}" "${accept_band}" \
    >> "${MATRIX_PATH}"
}

run_job() {
  local version="$1"
  local impl="$2"
  local points="$3"
  local warmup="$4"
  local repeat="$5"
  local accept_band
  accept_band="$(accept_band_for_points "${points}")"

  record_workload "${version}" "${impl}" "${points}" "${warmup}" "${repeat}"
  log_status "START version=${version} impl=${impl} points=${points} warmup=${warmup} repeat=${repeat} accept_band=${accept_band}%"

  DFT_IMPL="${impl}" \
  FFT_VALIDATE=0 \
  FFT_POINTS="${points}" \
  FFT_REPEAT="${repeat}" \
  FFT_WARMUP_REPEAT="${warmup}" \
  FFT_RUN_IDFT=0 \
  REPORT_ROOT="${STAGE_ROOT}" \
  JOBS="${JOBS}" \
  KEEP_BUILD="${KEEP_BUILD}" \
  "${SCRIPT_DIR}/run.sh"

  log_status "DONE version=${version} impl=${impl} points=${points} warmup=${warmup} repeat=${repeat} accept_band=${accept_band}%"
}

run_canonical_workloads() {
  # Scalar published baselines follow the unified canonical matrix.
  run_job scalar dft_scalar 16 8 30
  run_job scalar dft_scalar 64 8 10
  run_job scalar dft_scalar 128 8 10
  run_job scalar dft_scalar 256 6 10
  run_job scalar dft_scalar 512 6 10
  run_job scalar dft_scalar 1024 6 5
  run_job scalar dft_scalar 2048 4 3
  run_job scalar dft_scalar 4096 4 3

  # RVV published baselines follow the same point-level warmup policy.
  run_job rvv dft_rvv 16 8 30
  run_job rvv dft_rvv 64 8 10
  run_job rvv dft_rvv 128 8 10
  run_job rvv dft_rvv 256 6 10
  run_job rvv dft_rvv 512 6 10
  run_job rvv dft_rvv 1024 6 5
  run_job rvv dft_rvv 2048 4 3
  run_job rvv dft_rvv 4096 4 3
}

log_status "Canonical baseline root prepared at ${STAGE_ROOT}"
log_status "Matrix will be recorded at ${MATRIX_PATH}"

run_canonical_workloads

log_status "Canonical baseline sweep complete. Reports are under ${STAGE_ROOT}"
