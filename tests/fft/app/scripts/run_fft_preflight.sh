#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
XS_ROOT="$(cd -- "${APP_DIR}/../../.." && pwd)"
EVIDENCE_ROOT="${FFT_TEST_OUTPUT_ROOT:-${APP_DIR}/evidence}"
NEMU_BIN="${XS_ROOT}/NEMU/build/riscv64-nemu-interpreter"

usage() {
  printf 'usage: %s ARTIFACT_ROOT IMPL POINTS IDFT CORPUS_ID\n' "$0" >&2
  exit 2
}

[[ $# -eq 5 ]] || usage
ARTIFACT_ROOT="$1"
DFT_IMPL="$2"
FFT_POINTS="$3"
FFT_RUN_IDFT="$4"
CORPUS_ID="$5"
EXTRA_CPPFLAGS="${FFT_PREFLIGHT_EXTRA_CPPFLAGS:-}"

case "$DFT_IMPL" in
  dft_scalar|dft_rvv|dft_ext) ;;
  *) echo "unsupported implementation: ${DFT_IMPL}" >&2; exit 2 ;;
esac
case "$FFT_POINTS" in
  16|64|128|256|512|1024|2048|4096) ;;
  *) echo "unsupported points: ${FFT_POINTS}" >&2; exit 2 ;;
esac
case "$FFT_RUN_IDFT" in
  0|1) ;;
  *) echo "unsupported idft: ${FFT_RUN_IDFT}" >&2; exit 2 ;;
esac

resolved_root="$(realpath -m -- "$ARTIFACT_ROOT")"
resolved_evidence="$(realpath -m -- "$EVIDENCE_ROOT")"
case "$resolved_root/" in
  "$resolved_evidence"/*) ;;
  *) echo "refusing output outside evidence: ${resolved_root}" >&2; exit 3 ;;
esac

if [[ -e "$resolved_root" ]]; then
  if [[ ! -d "$resolved_root/state" ]]; then
    echo "refusing to overwrite existing artifact root: ${resolved_root}" >&2
    exit 3
  fi
  if find "$resolved_root" -mindepth 1 -maxdepth 1 ! -name state -print -quit | grep -q .; then
    echo "refusing non-empty artifact root: ${resolved_root}" >&2
    exit 3
  fi
else
  mkdir -p "$resolved_root"
fi
mkdir -p \
  "$resolved_root/build/objects" \
  "$resolved_root/corpus" \
  "$resolved_root/logs" \
  "$resolved_root/correctness" \
  "$resolved_root/manifests" \
  "$resolved_root/state"

for state_file in pid start_time exit_code end_time wall_seconds status; do
  if [[ -e "$resolved_root/state/$state_file" ]]; then
    echo "refusing to overwrite state file: ${resolved_root}/state/${state_file}" >&2
    exit 3
  fi
done

start_epoch="$(date +%s)"
start_iso="$(date --iso-8601=seconds)"
printf '%s\n' "$$" > "$resolved_root/state/pid"
printf '%s\n' "$start_iso" > "$resolved_root/state/start_time"

finish() {
  rc=$?
  printf '%s\n' "$rc" > "$resolved_root/state/exit_code"
  printf '%s\n' "$(date --iso-8601=seconds)" > "$resolved_root/state/end_time"
  printf '%s\n' "$(( $(date +%s) - start_epoch ))" > "$resolved_root/state/wall_seconds"
  if [[ "$rc" -eq 0 ]]; then
    printf 'PASS\n' > "$resolved_root/state/status"
  else
    printf 'FAIL\n' > "$resolved_root/state/status"
  fi
}
trap finish EXIT

if [[ ! -f "$NEMU_BIN" ]]; then
  echo "missing NEMU: ${NEMU_BIN}" >&2
  exit 4
fi
for required in \
  "$XS_ROOT/nexus-am/am/build/am-riscv64-xs.a" \
  "$XS_ROOT/nexus-am/libs/klib/build/klib-riscv64-xs.a"; do
  [[ -f "$required" ]] || { echo "missing shared archive: ${required}" >&2; exit 4; }
done

(
  cd "$APP_DIR"
  find . -type f \
    ! -path './build/*' \
    ! -path './evidence/*' \
    ! -path './archive/*' \
    ! -path './docs/*' \
    ! -path './.vscode/*' \
    ! -path './scripts/__pycache__/*' \
    -print0 | sort -z | xargs -0 sha256sum
) > "$resolved_root/manifests/source.sha256"

if [[ -n "${FFT_PREFLIGHT_EXPECTED_SOURCE_MANIFEST:-}" ]]; then
  [[ -f "$FFT_PREFLIGHT_EXPECTED_SOURCE_MANIFEST" ]] || {
    echo "missing expected source manifest: $FFT_PREFLIGHT_EXPECTED_SOURCE_MANIFEST" >&2
    exit 7
  }
  if ! cmp -s "$resolved_root/manifests/source.sha256" "$FFT_PREFLIGHT_EXPECTED_SOURCE_MANIFEST"; then
    echo "source manifest drift detected during preflight: $resolved_root" >&2
    exit 7
  fi
  printf '%s\n' "$FFT_PREFLIGHT_EXPECTED_SOURCE_MANIFEST" \
    > "$resolved_root/manifests/expected-source-manifest.txt"
fi

sha256sum \
  "$XS_ROOT/nexus-am/am/build/am-riscv64-xs.a" \
  "$XS_ROOT/nexus-am/libs/klib/build/klib-riscv64-xs.a" \
  > "$resolved_root/manifests/shared-archives-before.sha256"

python3 "$SCRIPT_DIR/generate_fft_corpus.py" \
  --corpus "$CORPUS_ID" \
  --points "$FFT_POINTS" \
  --output "$resolved_root/corpus/data" \
  --overlay "$resolved_root/overlay" \
  > "$resolved_root/corpus/generator.json"

build_base="$resolved_root/build/fft-${DFT_IMPL}-n${FFT_POINTS}-idft${FFT_RUN_IDFT}"
binary="$build_base.bin"
build_command="$resolved_root/manifests/build-command.txt"
printf '%s\n' \
  "cd ${XS_ROOT}" \
  "source ./env.sh" \
  "make -C ${APP_DIR} -o am -o klib -o prompt ARCH=riscv64-xs DFT_IMPL=${DFT_IMPL} FFT_EVIDENCE_ONLY=1 FFT_POINTS=${FFT_POINTS} FFT_REPEAT=1 FFT_WARMUP_REPEAT=1 FFT_RUN_IDFT=${FFT_RUN_IDFT} FFT_VALIDATE=1 FFT_ROI_HPM_ENABLE=0 CPPFLAGS=-I${resolved_root}/overlay\\ ${EXTRA_CPPFLAGS} DST_DIR=${resolved_root}/build/objects/ BINARY=${build_base} -j1" \
  > "$build_command"

(
  cd "$XS_ROOT"
  source ./env.sh >/dev/null
  make -C "$APP_DIR" \
    -o am -o klib -o prompt \
    ARCH=riscv64-xs \
    DFT_IMPL="$DFT_IMPL" \
    FFT_EVIDENCE_ONLY=1 \
    FFT_POINTS="$FFT_POINTS" \
    FFT_REPEAT=1 \
    FFT_WARMUP_REPEAT=1 \
    FFT_RUN_IDFT="$FFT_RUN_IDFT" \
    FFT_VALIDATE=1 \
    FFT_ROI_HPM_ENABLE=0 \
    CPPFLAGS="-I${resolved_root}/overlay -DFFT_EXP_FAIL_PRINT_LIMIT=8 ${EXTRA_CPPFLAGS}" \
    DST_DIR="${resolved_root}/build/objects/" \
    BINARY="$build_base" \
    -j1
) > "$resolved_root/logs/build.log" 2>&1

for artifact in "$binary" "$build_base.elf" "$build_base.txt"; do
  [[ -f "$artifact" ]] || { echo "missing build artifact: ${artifact}" >&2; exit 5; }
done
sha256sum "$binary" "$build_base.elf" "$build_base.txt" "$resolved_root"/build/objects/*.o \
  > "$resolved_root/manifests/binary.sha256"

sha256sum \
  "$XS_ROOT/nexus-am/am/build/am-riscv64-xs.a" \
  "$XS_ROOT/nexus-am/libs/klib/build/klib-riscv64-xs.a" \
  > "$resolved_root/manifests/shared-archives-after.sha256"
if ! cmp -s \
  "$resolved_root/manifests/shared-archives-before.sha256" \
  "$resolved_root/manifests/shared-archives-after.sha256"; then
  echo "shared AM/klib archive changed; refusing to accept run" >&2
  exit 6
fi

nemu_log="$resolved_root/logs/nemu.log"
set +e
(
  cd "$resolved_root/logs"
  "$NEMU_BIN" -b "$binary"
) > "$nemu_log" 2>&1
nemu_rc=$?
set -e
printf '%s\n' "$nemu_rc" > "$resolved_root/logs/nemu.exit_code"

python3 "$SCRIPT_DIR/parse_fft_validation.py" \
  --log "$nemu_log" \
  --output "$resolved_root/correctness/result.json" \
  --binary "$binary" \
  --source-manifest "$resolved_root/manifests/source.sha256" \
  --corpus-manifest "$resolved_root/corpus/data/corpus.json" \
  --impl "$DFT_IMPL" \
  --points "$FFT_POINTS" \
  --idft "$FFT_RUN_IDFT" \
  --nemu-exit "$nemu_rc"

printf '%s\n' \
  "candidate_root=${resolved_root}" \
  "impl=${DFT_IMPL}" \
  "points=${FFT_POINTS}" \
  "idft=${FFT_RUN_IDFT}" \
  "corpus=${CORPUS_ID}" \
  "validate=1" \
  "warmup=1" \
  "repeat=1" \
  "official=0" \
  "evidence_only=1" \
  "extra_cppflags=${EXTRA_CPPFLAGS}" \
  "output_root=${resolved_evidence}" \
  > "$resolved_root/manifests/run.env"
