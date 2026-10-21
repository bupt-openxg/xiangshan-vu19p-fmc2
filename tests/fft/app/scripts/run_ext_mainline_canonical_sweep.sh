#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
TIMESTAMP="${TIMESTAMP:-$(date +%Y%m%dT%H%M%S)}"
STAGE_BASE="${STAGE_BASE:-/tmp/fft-ext-mainline-canonical}"
STAGE_ROOT="${STAGE_ROOT:-${STAGE_BASE}/${TIMESTAMP}}"
VALIDATE_ROOT="${VALIDATE_ROOT:-${STAGE_ROOT}/validate}"
SWEEP_ROOT="${SWEEP_ROOT:-${STAGE_ROOT}/sweep}"
FINAL_ROOT="${FINAL_ROOT:-${APP_DIR}/perf_eval}"
JOBS="${JOBS:-8}"
KEEP_BUILD="${KEEP_BUILD:-1}"
RUN_EXT_VALIDATE="${RUN_EXT_VALIDATE:-1}"
RUN_EXT_SWEEP="${RUN_EXT_SWEEP:-1}"
PUBLISH_TO_FINAL_ROOT="${PUBLISH_TO_FINAL_ROOT:-1}"
GENERATE_REPORTS="${GENERATE_REPORTS:-1}"

STATUS_PATH="${STAGE_ROOT}/status.log"
README_PATH="${STAGE_ROOT}/README.txt"
VALIDATION_MATRIX_PATH="${STAGE_ROOT}/validation-matrix.tsv"
SWEEP_MATRIX_PATH="${STAGE_ROOT}/sweep-matrix.tsv"

mkdir -p "${STAGE_ROOT}" "${STAGE_BASE}" "${VALIDATE_ROOT}" "${SWEEP_ROOT}"
ln -sfn "${STAGE_ROOT}" "${STAGE_BASE}/latest"

cat > "${README_PATH}" <<EOF
EXT mainline canonical sweep
timestamp=${TIMESTAMP}
stage_root=${STAGE_ROOT}
validate_root=${VALIDATE_ROOT}
sweep_root=${SWEEP_ROOT}
final_root=${FINAL_ROOT}
run_ext_validate=${RUN_EXT_VALIDATE}
run_ext_sweep=${RUN_EXT_SWEEP}
publish_to_final_root=${PUBLISH_TO_FINAL_ROOT}
generate_reports=${GENERATE_REPORTS}

Policy:
- validate current EXT mainline on NEMU before publishing perf
- perf sweep follows the formal steady-state standard under perf_eval/overview
- staged artifacts live outside perf_eval root; final publish keeps perf_eval clean
- this stage publishes raw perf reports for ext and can refresh the maintained ext overview page

Canonical matrix:
- N=16:   warmup 8, measured repeat 30
- N=64:   warmup 8, measured repeat 10
- N=128:  warmup 8, measured repeat 10
- N=256:  warmup 6, measured repeat 10
- N=512:  warmup 6, measured repeat 10
- N=1024: warmup 6, measured repeat 5
- N=2048: warmup 4, measured repeat 3
- N=4096: warmup 4, measured repeat 3
EOF

printf "impl\tpoints\tidft\tstatus\n" > "${VALIDATION_MATRIX_PATH}"
printf "impl\tpoints\twarmup\trepeat\tstatus\n" > "${SWEEP_MATRIX_PATH}"

log_status() {
  printf '[%s] %s\n' "$(date '+%F %T')" "$*" | tee -a "${STATUS_PATH}"
}

canonical_warmup_for_points() {
  local points="$1"
  case "${points}" in
    16|64|128) printf '8' ;;
    256|512|1024) printf '6' ;;
    2048|4096) printf '4' ;;
    *)
      echo "Unsupported points=${points}" >&2
      exit 1
      ;;
  esac
}

canonical_repeat_for_points() {
  local points="$1"
  case "${points}" in
    16) printf '30' ;;
    64|128|256|512) printf '10' ;;
    1024) printf '5' ;;
    2048|4096) printf '3' ;;
    *)
      echo "Unsupported points=${points}" >&2
      exit 1
      ;;
  esac
}

record_validation() {
  local impl="$1"
  local points="$2"
  local idft="$3"
  local status="$4"
  printf "%s\t%s\t%s\t%s\n" "${impl}" "${points}" "${idft}" "${status}" >> "${VALIDATION_MATRIX_PATH}"
}

record_sweep() {
  local impl="$1"
  local points="$2"
  local warmup="$3"
  local repeat="$4"
  local status="$5"
  printf "%s\t%s\t%s\t%s\t%s\n" "${impl}" "${points}" "${warmup}" "${repeat}" "${status}" >> "${SWEEP_MATRIX_PATH}"
}

run_validate_job() {
  local points="$1"
  local idft="$2"

  log_status "VALIDATE START impl=dft_ext points=${points} idft=${idft}"
  if DFT_IMPL=dft_ext \
     FFT_VALIDATE=1 \
     FFT_POINTS="${points}" \
     FFT_REPEAT=1 \
     FFT_WARMUP_REPEAT=2 \
     FFT_RUN_IDFT="${idft}" \
     REPORT_ROOT="${VALIDATE_ROOT}" \
     JOBS="${JOBS}" \
     KEEP_BUILD="${KEEP_BUILD}" \
     "${SCRIPT_DIR}/run.sh"; then
    record_validation dft_ext "${points}" "${idft}" "PASS"
    log_status "VALIDATE DONE impl=dft_ext points=${points} idft=${idft}"
  else
    record_validation dft_ext "${points}" "${idft}" "FAIL"
    log_status "VALIDATE FAIL impl=dft_ext points=${points} idft=${idft}"
    exit 1
  fi
}

run_validation_matrix() {
  local points
  local idft

  for idft in 0 1; do
    for points in 16 64 128 256 512 1024 2048 4096; do
      run_validate_job "${points}" "${idft}"
    done
  done
}

run_ext_perf_job() {
  local points="$1"
  local warmup="$2"
  local repeat="$3"

  log_status "SWEEP START impl=dft_ext points=${points} warmup=${warmup} repeat=${repeat}"
  if DFT_IMPL=dft_ext \
     FFT_VALIDATE=0 \
     FFT_POINTS="${points}" \
     FFT_REPEAT="${repeat}" \
     FFT_WARMUP_REPEAT="${warmup}" \
     FFT_RUN_IDFT=0 \
     REPORT_ROOT="${SWEEP_ROOT}" \
     JOBS="${JOBS}" \
     KEEP_BUILD="${KEEP_BUILD}" \
     "${SCRIPT_DIR}/run.sh"; then
    record_sweep dft_ext "${points}" "${warmup}" "${repeat}" "PASS"
    log_status "SWEEP DONE impl=dft_ext points=${points} warmup=${warmup} repeat=${repeat}"
  else
    record_sweep dft_ext "${points}" "${warmup}" "${repeat}" "FAIL"
    log_status "SWEEP FAIL impl=dft_ext points=${points} warmup=${warmup} repeat=${repeat}"
    exit 1
  fi
}

run_ext_sweep() {
  local points
  local warmup
  local repeat

  for points in 16 64 128 256 512 1024 2048 4096; do
    warmup="$(canonical_warmup_for_points "${points}")"
    repeat="$(canonical_repeat_for_points "${points}")"
    run_ext_perf_job "${points}" "${warmup}" "${repeat}"
  done
}

publish_ext_stage() {
  local point_dir
  local point_name
  local final_sim_dir
  local report_path

  log_status "PUBLISH START final_root=${FINAL_ROOT}"
  mkdir -p "${FINAL_ROOT}/overview"

  for point_dir in "${FINAL_ROOT}"/n*; do
    [[ -d "${point_dir}" ]] || continue
    rm -rf "${point_dir}/ext"
  done

  for point_dir in "${SWEEP_ROOT}"/n*; do
    [[ -d "${point_dir}" ]] || continue
    point_name="$(basename "${point_dir}")"
    [[ -d "${point_dir}/ext/sim/mainline" ]] || continue

    final_sim_dir="${FINAL_ROOT}/${point_name}/ext/sim/mainline"
    mkdir -p "${final_sim_dir}"

    while IFS= read -r -d '' report_path; do
      cp -a "${report_path}" "${final_sim_dir}/"
    done < <(find "${point_dir}/ext/sim/mainline" -type f -name '*.report.txt' -print0)
  done

  log_status "PUBLISH DONE final_root=${FINAL_ROOT}"
}

generate_reports() {
  log_status "REPORT START run_dir=${FINAL_ROOT}"
  "${SCRIPT_DIR}/gen_fft_ext_reports.py" --run-dir "${FINAL_ROOT}"
  log_status "REPORT DONE run_dir=${FINAL_ROOT}"
}

log_status "Stage prepared at ${STAGE_ROOT}"

if [[ "${RUN_EXT_VALIDATE}" == "1" ]]; then
  run_validation_matrix
fi

if [[ "${RUN_EXT_SWEEP}" == "1" ]]; then
  run_ext_sweep
fi

if [[ "${PUBLISH_TO_FINAL_ROOT}" == "1" ]]; then
  publish_ext_stage
fi

if [[ "${GENERATE_REPORTS}" == "1" ]]; then
  generate_reports
fi

log_status "EXT mainline canonical sweep complete"
