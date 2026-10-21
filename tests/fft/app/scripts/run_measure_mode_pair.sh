#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
TIMESTAMP="${TIMESTAMP:-$(date +%Y%m%dT%H%M%S)}"
STAGE_BASE="${STAGE_BASE:-/tmp/fft-dual-mode-framework}"
STAGE_ROOT="${STAGE_ROOT:-${STAGE_BASE}/${TIMESTAMP}}"
DFT_IMPL="${DFT_IMPL:-dft_rvv}"
FFT_POINTS="${FFT_POINTS:-2048}"
FFT_REPEAT="${FFT_REPEAT:-3}"
FFT_WARMUP_REPEAT="${FFT_WARMUP_REPEAT:-4}"
FFT_RUN_IDFT="${FFT_RUN_IDFT:-0}"
JOBS="${JOBS:-8}"
KEEP_BUILD="${KEEP_BUILD:-1}"
PUBLISH_IMPL_NAME="${PUBLISH_IMPL_NAME:-publish}"
DIAG_IMPL_NAME="${DIAG_IMPL_NAME:-diag-backend}"
STATUS_PATH="${STAGE_ROOT}/status.log"
README_PATH="${STAGE_ROOT}/README.txt"

mkdir -p "${STAGE_ROOT}"
mkdir -p "${STAGE_BASE}"
ln -sfn "${STAGE_ROOT}" "${STAGE_BASE}/latest"

version_name() {
  case "$1" in
    oai) echo "oai" ;;
    dft_scalar) echo "scalar" ;;
    dft_rvv) echo "rvv" ;;
    *)
      echo "Unsupported DFT_IMPL=$1" >&2
      exit 1
      ;;
  esac
}

VERSION_NAME="$(version_name "${DFT_IMPL}")"

cat > "${README_PATH}" <<EOF
FFT dual-mode measurement pair
timestamp=${TIMESTAMP}
stage_root=${STAGE_ROOT}
version=${VERSION_NAME}
dft_impl=${DFT_IMPL}
points=${FFT_POINTS}
repeat=${FFT_REPEAT}
warmup_repeat=${FFT_WARMUP_REPEAT}
run_idft=${FFT_RUN_IDFT}

Modes:
- publish      : FFT_ROI_HPM_ENABLE=1, FFT_ROI_HPM_PROFILE=canonical
- diagnose     : FFT_ROI_HPM_ENABLE=1, FFT_ROI_HPM_PROFILE=backend_diag

Notes:
- this stage is additive and does not overwrite maintained mainline reports
- publish mode remains the authoritative ROI scope for final tables
- diagnose mode is an auxiliary ROI run used only for bottleneck attribution
EOF

log_status() {
  printf '[%s] %s\n' "$(date '+%F %T')" "$*" | tee -a "${STATUS_PATH}"
}

run_mode() {
  local impl_name="$1"
  local hpm_enable="$2"
  local hpm_profile="$3"
  local mode_name="$4"

  log_status "START mode=${mode_name} impl=${impl_name} points=${FFT_POINTS} repeat=${FFT_REPEAT} warmup=${FFT_WARMUP_REPEAT} hpm_enable=${hpm_enable} hpm_profile=${hpm_profile}"

  DFT_IMPL="${DFT_IMPL}" \
  FFT_IMPL_NAME="${impl_name}" \
  FFT_VALIDATE=0 \
  FFT_POINTS="${FFT_POINTS}" \
  FFT_REPEAT="${FFT_REPEAT}" \
  FFT_WARMUP_REPEAT="${FFT_WARMUP_REPEAT}" \
  FFT_RUN_IDFT="${FFT_RUN_IDFT}" \
  FFT_ROI_HPM_ENABLE="${hpm_enable}" \
  FFT_ROI_HPM_PROFILE="${hpm_profile}" \
  REPORT_ROOT="${STAGE_ROOT}" \
  JOBS="${JOBS}" \
  KEEP_BUILD="${KEEP_BUILD}" \
  "${SCRIPT_DIR}/run.sh"

  log_status "DONE mode=${mode_name} impl=${impl_name}"
}

log_status "Dual-mode measurement stage prepared at ${STAGE_ROOT}"

run_mode "${PUBLISH_IMPL_NAME}" 1 canonical publish
run_mode "${DIAG_IMPL_NAME}" 1 backend_diag diagnose

analysis_dir="${STAGE_ROOT}/n${FFT_POINTS}/${VERSION_NAME}/analysis/diag-framework"
output_path="${analysis_dir}/n${FFT_POINTS}-r${FFT_REPEAT}-idft${FFT_RUN_IDFT}-${VERSION_NAME}-dual-mode.analysis.md"
mkdir -p "${analysis_dir}"

"${SCRIPT_DIR}/gen_fft_measure_mode_report.py" \
  --run-dir "${STAGE_ROOT}" \
  --version "${VERSION_NAME}" \
  --points "${FFT_POINTS}" \
  --repeat "${FFT_REPEAT}" \
  --publish-impl "${PUBLISH_IMPL_NAME}" \
  --diag-impl "${DIAG_IMPL_NAME}" \
  --output "${output_path}"

log_status "Analysis report generated at ${output_path}"
