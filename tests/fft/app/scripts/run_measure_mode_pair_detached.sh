#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
STATE_ROOT="${FFT_DETACHED_STATE_ROOT:-/tmp/codex-fft-detached}"
STAGE_BASE="${STAGE_BASE:-/tmp/fft-dual-mode-framework}"
STAGE_ROOT="${STAGE_ROOT:-${STAGE_BASE}/$(date +%Y%m%dT%H%M%S)-${DFT_IMPL:-dft_rvv}-n${FFT_POINTS:-2048}-r${FFT_REPEAT:-3}}"

DFT_IMPL="${DFT_IMPL:-dft_rvv}"
FFT_POINTS="${FFT_POINTS:-2048}"
FFT_REPEAT="${FFT_REPEAT:-3}"
FFT_WARMUP_REPEAT="${FFT_WARMUP_REPEAT:-4}"
FFT_RUN_IDFT="${FFT_RUN_IDFT:-0}"
JOBS="${JOBS:-8}"
KEEP_BUILD="${KEEP_BUILD:-1}"
REPORT_ROOT="${REPORT_ROOT:-}"
PUBLISH_IMPL_NAME="${PUBLISH_IMPL_NAME:-publish}"
DIAG_IMPL_NAME="${DIAG_IMPL_NAME:-diag-backend}"

timestamp="$(date +%Y%m%dT%H%M%S)"
job_name="dual-mode-${DFT_IMPL}-n${FFT_POINTS}-r${FFT_REPEAT}-idft${FFT_RUN_IDFT}"
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
export DFT_IMPL='${DFT_IMPL}'
export FFT_POINTS='${FFT_POINTS}'
export FFT_REPEAT='${FFT_REPEAT}'
export FFT_WARMUP_REPEAT='${FFT_WARMUP_REPEAT}'
export FFT_RUN_IDFT='${FFT_RUN_IDFT}'
export JOBS='${JOBS}'
export KEEP_BUILD='${KEEP_BUILD}'
export REPORT_ROOT='${REPORT_ROOT}'
export STAGE_BASE='${STAGE_BASE}'
export STAGE_ROOT='${STAGE_ROOT}'
export PUBLISH_IMPL_NAME='${PUBLISH_IMPL_NAME}'
export DIAG_IMPL_NAME='${DIAG_IMPL_NAME}'
if bash ./run_measure_mode_pair.sh > "${log_path}" 2>&1; then
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
report_root=${REPORT_ROOT}
stage_root=${STAGE_ROOT}
dft_impl=${DFT_IMPL}
points=${FFT_POINTS}
repeat=${FFT_REPEAT}
warmup_repeat=${FFT_WARMUP_REPEAT}
publish_impl_name=${PUBLISH_IMPL_NAME}
diag_impl_name=${DIAG_IMPL_NAME}
EOF

printf 'Detached FFT dual-mode job started.\n'
printf 'pid=%s\n' "${pid}"
printf 'job_dir=%s\n' "${job_dir}"
printf 'log_path=%s\n' "${log_path}"
printf 'rc_path=%s\n' "${rc_path}"
printf 'done_path=%s\n' "${done_path}"
