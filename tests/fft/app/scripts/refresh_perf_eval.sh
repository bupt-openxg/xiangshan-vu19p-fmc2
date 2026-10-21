#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
FINAL_ROOT="${FINAL_ROOT:-${APP_DIR}/perf_eval}"
STAGE_ROOT="${STAGE_ROOT:-$(mktemp -d /tmp/fft-perf-refresh.XXXXXX)}"
JOBS="${JOBS:-8}"
KEEP_BUILD="${KEEP_BUILD:-1}"
RUN_CANONICAL_SWEEP="${RUN_CANONICAL_SWEEP:-1}"

generate_reports() {
  echo
  echo "=== Generate scalar reports ==="
  "${SCRIPT_DIR}/gen_fft_perf_reports.py" --run-dir "${FINAL_ROOT}"

  echo
  echo "=== Generate RVV reports ==="
  "${SCRIPT_DIR}/gen_fft_rvv_reports.py" --run-dir "${FINAL_ROOT}"
}

publish_stage() {
  echo
  echo "=== Publish staged reports into ${FINAL_ROOT} ==="

  find "${FINAL_ROOT}" -mindepth 1 -maxdepth 1 -type d -name 'n*' -exec rm -rf {} +
  mkdir -p "${FINAL_ROOT}/overview"
  find "${FINAL_ROOT}" -type f \( -name '*.validate-nemu.txt' -o -name '*.nemu.txt' -o -name '*.perf-nemu.txt' \) -delete
  find "${FINAL_ROOT}" -mindepth 1 -maxdepth 1 -type d -name 'exp-*' -exec rm -rf {} +
  find "${FINAL_ROOT}" -mindepth 1 -maxdepth 1 -type f -name '*.analysis.md' -delete

  for point_dir in "${STAGE_ROOT}"/n*; do
    [[ -d "${point_dir}" ]] || continue
    point_name="$(basename "${point_dir}")"

    for version_dir in "${point_dir}"/*; do
      [[ -d "${version_dir}" ]] || continue
      version_name="$(basename "${version_dir}")"
      final_sim_dir="${FINAL_ROOT}/${point_name}/${version_name}/sim"
      mkdir -p "${final_sim_dir}"

      while IFS= read -r -d '' report_path; do
        cp -a "${report_path}" "${final_sim_dir}/"
      done < <(find "${version_dir}/sim" -type f -name '*.report.txt' -print0)
    done
  done
}

echo "Stage root: ${STAGE_ROOT}"

if [[ "${RUN_CANONICAL_SWEEP}" == "1" ]]; then
  STAGE_ROOT="${STAGE_ROOT}" \
  JOBS="${JOBS}" \
  KEEP_BUILD="${KEEP_BUILD}" \
  bash "${SCRIPT_DIR}/run_canonical_baseline_sweep.sh"
else
  echo "Skip canonical sweep; reuse existing stage root."
fi

publish_stage
generate_reports

echo
echo "Refresh complete."
echo "Published root: ${FINAL_ROOT}"
echo "Stage root kept at: ${STAGE_ROOT}"
