#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
STATE_ROOT="${FFT_DETACHED_STATE_ROOT:-/tmp/codex-fft-detached}"
STAGE_BASE="${STAGE_BASE:-/tmp/fft-scalar-warmup-probe}"
WAIT_FOR_COMPLETION="${WAIT_FOR_COMPLETION:-0}"
JOBS="${JOBS:-8}"
KEEP_BUILD="${KEEP_BUILD:-1}"

timestamp="$(date +%Y%m%dT%H%M%S)"
stage_root="${STAGE_BASE}/${timestamp}"
job_dir="${STATE_ROOT}/${timestamp}-scalar-warmup-probe"
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
export JOBS='${JOBS}'
export KEEP_BUILD='${KEEP_BUILD}'
if bash ./run_scalar_warmup_probe.sh > "${log_path}" 2>&1; then
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
stage_root=${stage_root}
matrix_path=${stage_root}/matrix.tsv
status_path=${stage_root}/status.log
readme_path=${stage_root}/README.txt
EOF

printf 'Detached FFT scalar warmup probe started.\n'
printf 'pid=%s\n' "${pid}"
printf 'job_dir=%s\n' "${job_dir}"
printf 'log_path=%s\n' "${log_path}"
printf 'rc_path=%s\n' "${rc_path}"
printf 'stage_root=%s\n' "${stage_root}"

if [[ "${WAIT_FOR_COMPLETION}" == "1" ]]; then
  while [[ ! -f "${done_path}" ]]; do
    sleep 5
  done
  rc="unknown"
  if [[ -f "${rc_path}" ]]; then
    rc="$(cat "${rc_path}")"
  fi
  printf 'Detached FFT scalar warmup probe finished.\n'
  printf 'pid=%s\n' "${pid}"
  printf 'rc=%s\n' "${rc}"
fi
