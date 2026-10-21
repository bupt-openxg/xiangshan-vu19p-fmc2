#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
STATE_ROOT="${FFT_DETACHED_STATE_ROOT:-/tmp/codex-fft-detached}"

ARCH="${ARCH:-riscv64-xs}"
DFT_IMPL="${DFT_IMPL:-oai}"
FFT_IMPL_NAME="${FFT_IMPL_NAME:-}"
FFT_POINTS="${FFT_POINTS:-16}"
FFT_REPEAT="${FFT_REPEAT:-1}"
FFT_RUN_IDFT="${FFT_RUN_IDFT:-0}"
FFT_VALIDATE="${FFT_VALIDATE:-1}"
JOBS="${JOBS:-8}"
KEEP_BUILD="${KEEP_BUILD:-0}"
REPORT_ROOT="${REPORT_ROOT:-}"
FFT_WARMUP_REPEAT="${FFT_WARMUP_REPEAT:-2}"
WAIT_FOR_COMPLETION="${WAIT_FOR_COMPLETION:-0}"

timestamp="$(date +%Y%m%dT%H%M%S)"
job_name="${DFT_IMPL}-n${FFT_POINTS}-r${FFT_REPEAT}-idft${FFT_RUN_IDFT}"
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
export ARCH='${ARCH}'
export DFT_IMPL='${DFT_IMPL}'
export FFT_IMPL_NAME='${FFT_IMPL_NAME}'
export FFT_POINTS='${FFT_POINTS}'
export FFT_REPEAT='${FFT_REPEAT}'
export FFT_WARMUP_REPEAT='${FFT_WARMUP_REPEAT}'
export FFT_RUN_IDFT='${FFT_RUN_IDFT}'
export FFT_VALIDATE='${FFT_VALIDATE}'
export JOBS='${JOBS}'
export KEEP_BUILD='${KEEP_BUILD}'
export REPORT_ROOT='${REPORT_ROOT}'
if bash ./run.sh > "${log_path}" 2>&1; then
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
report_root=${REPORT_ROOT}
warmup_repeat=${FFT_WARMUP_REPEAT}
EOF

printf 'Detached FFT job started.\n'
printf 'pid=%s\n' "${pid}"
printf 'job_dir=%s\n' "${job_dir}"
printf 'log_path=%s\n' "${log_path}"
printf 'rc_path=%s\n' "${rc_path}"

if [[ "${WAIT_FOR_COMPLETION}" == "1" ]]; then
  while [[ ! -f "${done_path}" ]]; do
    sleep 5
  done
  rc="unknown"
  if [[ -f "${rc_path}" ]]; then
    rc="$(cat "${rc_path}")"
  fi
  printf 'Detached FFT job finished.\n'
  printf 'pid=%s\n' "${pid}"
  printf 'rc=%s\n' "${rc}"
fi
