#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
STATE_ROOT="${FFT_DETACHED_STATE_ROOT:-/tmp/codex-fft-detached}"
WAIT_FOR_COMPLETION="${WAIT_FOR_COMPLETION:-0}"

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

timestamp="$(date +%Y%m%dT%H%M%S)"
job_dir="${STATE_ROOT}/${timestamp}-refresh-perf-eval-scalar-rvv-ext"
log_path="${job_dir}/run.log"
rc_path="${job_dir}/exit_code.txt"
pid_path="${job_dir}/pid"
meta_path="${job_dir}/meta.env"
done_path="${job_dir}/done"
launcher_path="${job_dir}/launch.sh"

mkdir -p "${job_dir}"

cat > "${launcher_path}" <<EOF
#!/usr/bin/env bash
set -euo pipefail
printf "%s\n" "\$\$" > "${pid_path}"
cd "${SCRIPT_DIR}"
export FINAL_ROOT='${FINAL_ROOT}'
export STAGE_ROOT='${STAGE_ROOT}'
export BASELINE_STAGE_ROOT='${BASELINE_STAGE_ROOT}'
export EXT_STAGE_ROOT='${EXT_STAGE_ROOT}'
export JOBS='${JOBS}'
export KEEP_BUILD='${KEEP_BUILD}'
export RUN_CANONICAL_SWEEP='${RUN_CANONICAL_SWEEP}'
export RUN_EXT_VALIDATE='${RUN_EXT_VALIDATE}'
export RUN_EXT_SWEEP='${RUN_EXT_SWEEP}'
export PUBLISH_TO_FINAL_ROOT='${PUBLISH_TO_FINAL_ROOT}'
export GENERATE_EXT_REPORTS='${GENERATE_EXT_REPORTS}'
if bash ./refresh_perf_eval_scalar_rvv_ext.sh > "${log_path}" 2>&1; then
  rc=0
else
  rc=\$?
fi
printf "%s\n" "\$rc" > "${rc_path}"
touch "${done_path}"
exit "\$rc"
EOF

chmod +x "${launcher_path}"
if command -v setsid >/dev/null 2>&1; then
  setsid -f bash "${launcher_path}" >/dev/null 2>&1
else
  nohup bash "${launcher_path}" >/dev/null 2>&1 &
fi

pid="unknown"
for _ in $(seq 1 50); do
  if [[ -f "${pid_path}" ]]; then
    pid="$(cat "${pid_path}")"
    break
  fi
  sleep 0.1
done

cat > "${meta_path}" <<EOF
pid=${pid}
job_dir=${job_dir}
log_path=${log_path}
rc_path=${rc_path}
done_path=${done_path}
final_root=${FINAL_ROOT}
stage_root=${STAGE_ROOT}
baseline_stage_root=${BASELINE_STAGE_ROOT}
ext_stage_root=${EXT_STAGE_ROOT}
run_canonical_sweep=${RUN_CANONICAL_SWEEP}
run_ext_validate=${RUN_EXT_VALIDATE}
run_ext_sweep=${RUN_EXT_SWEEP}
publish_to_final_root=${PUBLISH_TO_FINAL_ROOT}
generate_ext_reports=${GENERATE_EXT_REPORTS}
EOF

printf 'Detached FFT scalar/rvv/ext refresh job started.\n'
printf 'pid=%s\n' "${pid}"
printf 'job_dir=%s\n' "${job_dir}"
printf 'log_path=%s\n' "${log_path}"
printf 'rc_path=%s\n' "${rc_path}"
printf 'done_path=%s\n' "${done_path}"
printf 'stage_root=%s\n' "${STAGE_ROOT}"
printf 'baseline_stage_root=%s\n' "${BASELINE_STAGE_ROOT}"
printf 'ext_stage_root=%s\n' "${EXT_STAGE_ROOT}"
printf 'final_root=%s\n' "${FINAL_ROOT}"

if [[ "${WAIT_FOR_COMPLETION}" == "1" ]]; then
  while [[ ! -f "${done_path}" ]]; do
    sleep 5
  done
  rc="unknown"
  if [[ -f "${rc_path}" ]]; then
    rc="$(cat "${rc_path}")"
  fi
  printf 'Detached FFT scalar/rvv/ext refresh job finished.\n'
  printf 'pid=%s\n' "${pid}"
  printf 'rc=%s\n' "${rc}"
fi
