#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
STATE_ROOT="${FFT_DETACHED_STATE_ROOT:-/tmp/codex-fft-detached}"
STAGE_BASE="${STAGE_BASE:-/tmp/fft-rvv-mainline-canonical}"
WAIT_FOR_COMPLETION="${WAIT_FOR_COMPLETION:-0}"
FINAL_ROOT="${FINAL_ROOT:-${APP_DIR}/perf_eval}"
JOBS="${JOBS:-8}"
KEEP_BUILD="${KEEP_BUILD:-1}"
RUN_RVV_VALIDATE="${RUN_RVV_VALIDATE:-1}"
RUN_RVV_SWEEP="${RUN_RVV_SWEEP:-1}"
PUBLISH_TO_FINAL_ROOT="${PUBLISH_TO_FINAL_ROOT:-1}"
GENERATE_REPORTS="${GENERATE_REPORTS:-1}"

timestamp="$(date +%Y%m%dT%H%M%S)"
stage_root="${STAGE_BASE}/${timestamp}"
job_name="rvv-mainline-canonical-sweep"
job_dir="${STATE_ROOT}/${timestamp}-${job_name}"
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
export TIMESTAMP='${timestamp}'
export STAGE_BASE='${STAGE_BASE}'
export STAGE_ROOT='${stage_root}'
export FINAL_ROOT='${FINAL_ROOT}'
export JOBS='${JOBS}'
export KEEP_BUILD='${KEEP_BUILD}'
export RUN_RVV_VALIDATE='${RUN_RVV_VALIDATE}'
export RUN_RVV_SWEEP='${RUN_RVV_SWEEP}'
export PUBLISH_TO_FINAL_ROOT='${PUBLISH_TO_FINAL_ROOT}'
export GENERATE_REPORTS='${GENERATE_REPORTS}'
if bash ./run_rvv_mainline_canonical_sweep.sh > "${log_path}" 2>&1; then
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
stage_root=${stage_root}
final_root=${FINAL_ROOT}
run_rvv_validate=${RUN_RVV_VALIDATE}
run_rvv_sweep=${RUN_RVV_SWEEP}
publish_to_final_root=${PUBLISH_TO_FINAL_ROOT}
generate_reports=${GENERATE_REPORTS}
EOF

printf 'Detached RVV mainline canonical sweep started.\n'
printf 'pid=%s\n' "${pid}"
printf 'job_dir=%s\n' "${job_dir}"
printf 'log_path=%s\n' "${log_path}"
printf 'rc_path=%s\n' "${rc_path}"
printf 'done_path=%s\n' "${done_path}"
printf 'stage_root=%s\n' "${stage_root}"
printf 'final_root=%s\n' "${FINAL_ROOT}"

if [[ "${WAIT_FOR_COMPLETION}" == "1" ]]; then
  while [[ ! -f "${done_path}" ]]; do
    sleep 5
  done
  rc="unknown"
  if [[ -f "${rc_path}" ]]; then
    rc="$(cat "${rc_path}")"
  fi
  printf 'Detached RVV mainline canonical sweep finished.\n'
  printf 'pid=%s\n' "${pid}"
  printf 'rc=%s\n' "${rc}"
fi
