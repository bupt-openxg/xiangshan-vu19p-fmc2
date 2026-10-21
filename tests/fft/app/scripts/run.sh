#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
XS_ROOT="$(cd -- "${APP_DIR}/../../.." && pwd)"

ARCH="${ARCH:-riscv64-xs}"
DFT_IMPL="${DFT_IMPL:-oai}"
FFT_POINTS="${FFT_POINTS:-16}"
FFT_REPEAT="${FFT_REPEAT:-1}"
FFT_WARMUP_REPEAT="${FFT_WARMUP_REPEAT:-2}"
FFT_RUN_IDFT="${FFT_RUN_IDFT:-0}"
FFT_VALIDATE="${FFT_VALIDATE:-1}"
FFT_ROI_HPM_ENABLE="${FFT_ROI_HPM_ENABLE:-1}"
FFT_ROI_HPM_PROFILE="${FFT_ROI_HPM_PROFILE:-canonical}"
FFT_IMPL_NAME="${FFT_IMPL_NAME:-}"
JOBS="${JOBS:-8}"
KEEP_BUILD="${KEEP_BUILD:-0}"
EMU_OUT="${EMU_OUT:-}"
REPORT_ROOT="${REPORT_ROOT:-}"
FFT_RUN_LOCK_ROOT="${FFT_RUN_LOCK_ROOT:-/tmp/codex-fft-run-locks}"
FFT_RUN_LOCK_PATH="${FFT_RUN_LOCK_ROOT}/global.lock"
FFT_RUN_LOCK_METHOD="none"

mode_tag="perf"
if [[ "${FFT_VALIDATE}" == "1" ]]; then
  mode_tag="validate"
fi

case "${DFT_IMPL}" in
  oai)
    version_name="oai"
    default_impl_name="minimal"
    bin_name="hello-oai-${mode_tag}-n${FFT_POINTS}-r${FFT_REPEAT}-idft${FFT_RUN_IDFT}-${ARCH}.bin"
    ;;
  dft_scalar)
    version_name="scalar"
    default_impl_name="mainline"
    bin_name="hello-scalar-${mode_tag}-n${FFT_POINTS}-r${FFT_REPEAT}-idft${FFT_RUN_IDFT}-${ARCH}.bin"
    ;;
  dft_rvv)
    version_name="rvv"
    default_impl_name="mainline"
    bin_name="hello-rvv-${mode_tag}-n${FFT_POINTS}-r${FFT_REPEAT}-idft${FFT_RUN_IDFT}-${ARCH}.bin"
    ;;
  dft_ext)
    version_name="ext"
    default_impl_name="mainline"
    bin_name="hello-ext-${mode_tag}-n${FFT_POINTS}-r${FFT_REPEAT}-idft${FFT_RUN_IDFT}-${ARCH}.bin"
    ;;
  *)
    echo "Unsupported DFT_IMPL=${DFT_IMPL}" >&2
    exit 1
    ;;
esac

if [[ -z "${FFT_IMPL_NAME}" ]]; then
  FFT_IMPL_NAME="${default_impl_name}"
fi

artifact_tag="${version_name}"
if [[ "${FFT_IMPL_NAME}" != "mainline" && "${FFT_IMPL_NAME}" != "minimal" ]]; then
  artifact_tag="${version_name}-${FFT_IMPL_NAME}"
fi

if [[ -n "${REPORT_ROOT}" ]]; then
  point_dir="${REPORT_ROOT}/n${FFT_POINTS}"
  version_dir="${point_dir}/${version_name}"
  sim_dir="${version_dir}/sim/${FFT_IMPL_NAME}"
  analysis_dir="${version_dir}/analysis/${FFT_IMPL_NAME}"
  mkdir -p "${sim_dir}" "${analysis_dir}"
fi

cleanup() {
  if [[ "${KEEP_BUILD}" != "1" ]]; then
    make -s -C "${APP_DIR}" clean >/dev/null 2>&1 || true
  else
    make -s -C "${APP_DIR}" tidy >/dev/null 2>&1 || true
  fi
}

acquire_run_lock() {
  mkdir -p "${FFT_RUN_LOCK_ROOT}"
  if command -v flock >/dev/null 2>&1; then
    exec 9>"${FFT_RUN_LOCK_PATH}"
    flock 9
    FFT_RUN_LOCK_METHOD="flock"
  else
    local lock_dir="${FFT_RUN_LOCK_PATH}.dir"
    while ! mkdir "${lock_dir}" 2>/dev/null; do
      sleep 1
    done
    FFT_RUN_LOCK_METHOD="mkdir"
  fi
}

release_run_lock() {
  if [[ "${FFT_RUN_LOCK_METHOD}" == "mkdir" ]]; then
    rmdir "${FFT_RUN_LOCK_PATH}.dir" >/dev/null 2>&1 || true
  fi
}

finish() {
  local rc=$?
  trap - EXIT
  cleanup
  release_run_lock
  exit "${rc}"
}

trap finish EXIT

acquire_run_lock

pushd "${XS_ROOT}" >/dev/null
source ./env.sh
popd >/dev/null

make -C "${APP_DIR}" \
  ARCH="${ARCH}" \
  DFT_IMPL="${DFT_IMPL}" \
  FFT_POINTS="${FFT_POINTS}" \
  FFT_REPEAT="${FFT_REPEAT}" \
  FFT_WARMUP_REPEAT="${FFT_WARMUP_REPEAT}" \
  FFT_RUN_IDFT="${FFT_RUN_IDFT}" \
  FFT_VALIDATE="${FFT_VALIDATE}" \
  FFT_ROI_HPM_ENABLE="${FFT_ROI_HPM_ENABLE}" \
  FFT_ROI_HPM_PROFILE="${FFT_ROI_HPM_PROFILE}" \
  -j"${JOBS}"

bin_path="${APP_DIR}/build/${bin_name}"

if [[ "${FFT_VALIDATE}" == "1" ]]; then
  validate_log="$(mktemp)"
  validate_targets=("${validate_log}")
  if [[ -n "${REPORT_ROOT}" ]]; then
    validate_targets+=("${sim_dir}/n${FFT_POINTS}-r${FFT_REPEAT}-idft${FFT_RUN_IDFT}-${artifact_tag}.validate-nemu.txt")
  fi
  "${XS_ROOT}/NEMU/build/riscv64-nemu-interpreter" -b "${bin_path}" 2>&1 | tee "${validate_targets[@]}"
  if ! grep -q "CHECK: PASS" "${validate_log}"; then
    rm -f "${validate_log}"
    echo "Validate run did not reach CHECK: PASS" >&2
    exit 1
  fi
  rm -f "${validate_log}"
else
  if [[ -z "${EMU_OUT}" && -n "${REPORT_ROOT}" ]]; then
    EMU_OUT="${sim_dir}/n${FFT_POINTS}-r${FFT_REPEAT}-idft${FFT_RUN_IDFT}-${artifact_tag}.report.txt"
  fi

  pushd "${XS_ROOT}" >/dev/null
  source ./env.sh
  popd >/dev/null

  if [[ -n "${EMU_OUT}" ]]; then
    "${XS_ROOT}/XiangShan/build/emu" --no-diff --force-dump-result -i "${bin_path}" > "${EMU_OUT}" 2>&1
  else
    "${XS_ROOT}/XiangShan/build/emu" --no-diff --force-dump-result -i "${bin_path}"
  fi
fi
