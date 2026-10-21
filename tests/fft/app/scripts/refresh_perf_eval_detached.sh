#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
STATE_ROOT="${FFT_DETACHED_STATE_ROOT:-/tmp/codex-fft-detached}"
WAIT_FOR_COMPLETION="${WAIT_FOR_COMPLETION:-0}"

FINAL_ROOT="${FINAL_ROOT:-${APP_DIR}/perf_eval}"
STAGE_ROOT="${STAGE_ROOT:-$(mktemp -d /tmp/fft-perf-refresh.XXXXXX)}"
JOBS="${JOBS:-8}"
KEEP_BUILD="${KEEP_BUILD:-1}"

timestamp="$(date +%Y%m%dT%H%M%S)"
job_dir="${STATE_ROOT}/${timestamp}-refresh-perf-eval"
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
export JOBS='${JOBS}'
export KEEP_BUILD='${KEEP_BUILD}'
if bash ./refresh_perf_eval.sh > "${log_path}" 2>&1; then
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
final_root=${FINAL_ROOT}
stage_root=${STAGE_ROOT}
EOF

printf 'Detached FFT refresh job started.\n'
printf 'pid=%s\n' "${pid}"
printf 'job_dir=%s\n' "${job_dir}"
printf 'log_path=%s\n' "${log_path}"
printf 'rc_path=%s\n' "${rc_path}"
printf 'stage_root=%s\n' "${STAGE_ROOT}"

if [[ "${WAIT_FOR_COMPLETION}" == "1" ]]; then
  while [[ ! -f "${done_path}" ]]; do
    sleep 5
  done
  rc="unknown"
  if [[ -f "${rc_path}" ]]; then
    rc="$(cat "${rc_path}")"
  fi
  printf 'Detached FFT refresh job finished.\n'
  printf 'pid=%s\n' "${pid}"
  printf 'rc=%s\n' "${rc}"
fi
