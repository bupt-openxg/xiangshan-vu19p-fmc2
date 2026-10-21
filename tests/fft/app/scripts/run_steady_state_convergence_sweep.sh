#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
TIMESTAMP="${TIMESTAMP:-$(date +%Y%m%dT%H%M%S)}"
STAGE_BASE="${STAGE_BASE:-/tmp/fft-steady-state-convergence}"
STAGE_ROOT="${STAGE_ROOT:-${STAGE_BASE}/${TIMESTAMP}}"
JOBS="${JOBS:-8}"
KEEP_BUILD="${KEEP_BUILD:-1}"
WARMUP_REPEAT="${WARMUP_REPEAT:-2}"
MATRIX_PATH="${STAGE_ROOT}/matrix.tsv"
STATUS_PATH="${STAGE_ROOT}/status.log"
README_PATH="${STAGE_ROOT}/README.txt"

mkdir -p "${STAGE_ROOT}"
mkdir -p "${STAGE_BASE}"
ln -sfn "${STAGE_ROOT}" "${STAGE_BASE}/latest"

cat > "${README_PATH}" <<EOF
FFT steady-state convergence sweep
timestamp=${TIMESTAMP}
stage_root=${STAGE_ROOT}
warmup_repeat=${WARMUP_REPEAT}

Policy:
- scalar baseline: DFT_IMPL=dft_scalar for every point 16-4096
- rvv baseline: DFT_IMPL=dft_rvv
- all perf runs use FFT_VALIDATE=0 and FFT_RUN_IDFT=0
- reports are stored under this stage root using the normal run.sh layout

Convergence matrix:
- N=16: repeats 20, 30, 40
- N=64/128/256/512: repeats 10, 20
- N=1024/2048/4096: repeats 3, 5, 7
- RVV currently covers 16/64/128/256/512/1024/2048/4096
EOF

printf "version\timpl\tpoints\twarmup\tmeasured_repeat\n" > "${MATRIX_PATH}"

log_status() {
  printf '[%s] %s\n' "$(date '+%F %T')" "$*" | tee -a "${STATUS_PATH}"
}

record_workload() {
  local version="$1"
  local impl="$2"
  local points="$3"
  local repeat="$4"
  printf "%s\t%s\t%s\t%s\t%s\n" \
    "${version}" "${impl}" "${points}" "${WARMUP_REPEAT}" "${repeat}" \
    >> "${MATRIX_PATH}"
}

run_job() {
  local version="$1"
  local impl="$2"
  local points="$3"
  local repeat="$4"

  record_workload "${version}" "${impl}" "${points}" "${repeat}"
  log_status "START version=${version} impl=${impl} points=${points} warmup=${WARMUP_REPEAT} repeat=${repeat}"

  DFT_IMPL="${impl}" \
  FFT_VALIDATE=0 \
  FFT_POINTS="${points}" \
  FFT_REPEAT="${repeat}" \
  FFT_WARMUP_REPEAT="${WARMUP_REPEAT}" \
  FFT_RUN_IDFT=0 \
  REPORT_ROOT="${STAGE_ROOT}" \
  JOBS="${JOBS}" \
  KEEP_BUILD="${KEEP_BUILD}" \
  "${SCRIPT_DIR}/run.sh"

  log_status "DONE version=${version} impl=${impl} points=${points} warmup=${WARMUP_REPEAT} repeat=${repeat}"
}

log_status "Sweep root prepared at ${STAGE_ROOT}"
log_status "Matrix will be recorded at ${MATRIX_PATH}"

# Scalar convergence sweep: one fixed baseline implementation across 16-4096.
for repeat in 20 30 40; do
  run_job scalar dft_scalar 16 "${repeat}"
done

for points in 64 128 256 512; do
  for repeat in 10 20; do
    run_job scalar dft_scalar "${points}" "${repeat}"
  done
done

for points in 1024 2048 4096; do
  for repeat in 3 5 7; do
    run_job scalar dft_scalar "${points}" "${repeat}"
  done
done

# RVV convergence sweep: maintained mainline only.
for repeat in 20 30 40; do
  run_job rvv dft_rvv 16 "${repeat}"
done

for points in 64 128 256 512; do
  for repeat in 10 20; do
    run_job rvv dft_rvv "${points}" "${repeat}"
  done
done

for points in 1024 2048 4096; do
  for repeat in 3 5 7; do
    run_job rvv dft_rvv "${points}" "${repeat}"
  done
done

log_status "Sweep complete. Reports are under ${STAGE_ROOT}"
