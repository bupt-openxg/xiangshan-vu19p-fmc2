#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
TIMESTAMP="${TIMESTAMP:-$(date +%Y%m%dT%H%M%S)}"
STAGE_BASE="${STAGE_BASE:-/tmp/fft-scalar-warmup-probe}"
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
FFT scalar warmup probe
timestamp=${TIMESTAMP}
stage_root=${STAGE_ROOT}

Purpose:
- check whether scalar repeat sensitivity is mainly caused by warmup=2 being too small
- representative points only: N=64, 256, 1024
- scalar mainline only
- same code path, but reports are separated by temporary impl name warmup-w<k>

Matrix:
- N=64   : warmup 2/4/6/8, repeat 10/20
- N=256  : warmup 2/4/6/8, repeat 10/20
- N=1024 : warmup 2/4/6/8, repeat 5/7
EOF

printf "version\timpl\tpoints\twarmup\tmeasured_repeat\timpl_name\n" > "${MATRIX_PATH}"

log_status() {
  printf '[%s] %s\n' "$(date '+%F %T')" "$*" | tee -a "${STATUS_PATH}"
}

record_workload() {
  local version="$1"
  local impl="$2"
  local points="$3"
  local warmup="$4"
  local repeat="$5"
  local impl_name="$6"
  printf "%s\t%s\t%s\t%s\t%s\t%s\n" \
    "${version}" "${impl}" "${points}" "${warmup}" "${repeat}" "${impl_name}" \
    >> "${MATRIX_PATH}"
}

run_job() {
  local version="$1"
  local impl="$2"
  local points="$3"
  local warmup="$4"
  local repeat="$5"
  local impl_name="warmup-w${warmup}"

  record_workload "${version}" "${impl}" "${points}" "${warmup}" "${repeat}" "${impl_name}"
  log_status "START version=${version} impl=${impl} points=${points} warmup=${warmup} repeat=${repeat} impl_name=${impl_name}"

  DFT_IMPL="${impl}" \
  FFT_IMPL_NAME="${impl_name}" \
  FFT_VALIDATE=0 \
  FFT_POINTS="${points}" \
  FFT_REPEAT="${repeat}" \
  FFT_WARMUP_REPEAT="${warmup}" \
  FFT_RUN_IDFT=0 \
  REPORT_ROOT="${STAGE_ROOT}" \
  JOBS="${JOBS}" \
  KEEP_BUILD="${KEEP_BUILD}" \
  "${SCRIPT_DIR}/run.sh"

  log_status "DONE version=${version} impl=${impl} points=${points} warmup=${warmup} repeat=${repeat} impl_name=${impl_name}"
}

log_status "Scalar warmup probe root prepared at ${STAGE_ROOT}"
log_status "Matrix will be recorded at ${MATRIX_PATH}"

for warmup in 2 4 6 8; do
  for repeat in 10 20; do
    run_job scalar dft_scalar 64 "${warmup}" "${repeat}"
  done
done

for warmup in 2 4 6 8; do
  for repeat in 10 20; do
    run_job scalar dft_scalar 256 "${warmup}" "${repeat}"
  done
done

for warmup in 2 4 6 8; do
  for repeat in 5 7; do
    run_job scalar dft_scalar 1024 "${warmup}" "${repeat}"
  done
done

log_status "Scalar warmup probe complete. Reports are under ${STAGE_ROOT}"
