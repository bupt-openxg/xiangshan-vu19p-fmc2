#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
XS_ROOT="$(cd -- "${APP_DIR}/../../.." && pwd)"
EVIDENCE_ROOT="${FFT_TEST_OUTPUT_ROOT:-${APP_DIR}/evidence}"
EMU_BIN="${XS_ROOT}/XiangShan/build/emu"

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
EXTRA_CPPFLAGS="${FFT_OFFICIAL_EXTRA_CPPFLAGS:-}"

case "$DFT_IMPL" in
  dft_scalar|dft_rvv|dft_ext) ;;
  *) echo "unsupported implementation: $DFT_IMPL" >&2; exit 2 ;;
esac
case "$FFT_POINTS" in
  16|64|128|256|512|1024|2048|4096) ;;
  *) echo "unsupported points: $FFT_POINTS" >&2; exit 2 ;;
esac
case "$FFT_RUN_IDFT" in
  0|1) ;;
  *) echo "unsupported idft: $FFT_RUN_IDFT" >&2; exit 2 ;;
esac
[[ "$CORPUS_ID" == C0-legacy-pattern-v1 ]] || {
  echo "official corpus must be C0-legacy-pattern-v1" >&2
  exit 2
}

case "$FFT_POINTS" in
  16) warmup=8; repeat=30 ;;
  64|128) warmup=8; repeat=10 ;;
  256|512) warmup=6; repeat=10 ;;
  1024) warmup=6; repeat=5 ;;
  2048|4096) warmup=4; repeat=3 ;;
esac

resolved_root="$(realpath -m -- "$ARTIFACT_ROOT")"
resolved_evidence="$(realpath -m -- "$EVIDENCE_ROOT")"
case "$resolved_root/" in
  "$resolved_evidence"/*) ;;
  *) echo "refusing output outside evidence: $resolved_root" >&2; exit 3 ;;
esac
if [[ -e "$resolved_root" ]]; then
  [[ -d "$resolved_root/state" ]] || {
    echo "refusing to overwrite existing artifact root: $resolved_root" >&2
    exit 3
  }
  if find "$resolved_root" -mindepth 1 -maxdepth 1 ! -name state -print -quit | grep -q .; then
    echo "refusing non-empty artifact root: $resolved_root" >&2
    exit 3
  fi
else
  mkdir -p "$resolved_root"
fi
mkdir -p "$resolved_root/build/objects" "$resolved_root/corpus" "$resolved_root/logs" "$resolved_root/official" "$resolved_root/manifests" "$resolved_root/state"
for state_file in pid start_time exit_code end_time wall_seconds status; do
  [[ ! -e "$resolved_root/state/$state_file" ]] || {
    echo "refusing to overwrite state file: $resolved_root/state/$state_file" >&2
    exit 3
  }
done

start_epoch="$(date +%s)"
printf '%s\n' "$$" > "$resolved_root/state/pid"
printf '%s\n' "$(date --iso-8601=seconds)" > "$resolved_root/state/start_time"
finish() {
  rc=$?
  printf '%s\n' "$rc" > "$resolved_root/state/exit_code"
  printf '%s\n' "$(date --iso-8601=seconds)" > "$resolved_root/state/end_time"
  printf '%s\n' "$(( $(date +%s) - start_epoch ))" > "$resolved_root/state/wall_seconds"
  if [[ "$rc" -eq 0 ]]; then printf 'PASS\n' > "$resolved_root/state/status"; else printf 'FAIL\n' > "$resolved_root/state/status"; fi
}
trap finish EXIT

[[ -f "$EMU_BIN" ]] || { echo "missing EMU: $EMU_BIN" >&2; exit 4; }
for required in "$XS_ROOT/nexus-am/am/build/am-riscv64-xs.a" "$XS_ROOT/nexus-am/libs/klib/build/klib-riscv64-xs.a"; do
  [[ -f "$required" ]] || { echo "missing shared archive: $required" >&2; exit 4; }
done

(
  cd "$APP_DIR"
  find . -type f \
    ! -path './build/*' ! -path './evidence/*' ! -path './archive/*' \
    ! -path './docs/*' ! -path './.vscode/*' ! -path './scripts/__pycache__/*' \
    -print0 | sort -z | xargs -0 sha256sum
) > "$resolved_root/manifests/source.sha256"

if [[ -n "${FFT_OFFICIAL_EXPECTED_SOURCE_MANIFEST:-}" ]]; then
  [[ -f "$FFT_OFFICIAL_EXPECTED_SOURCE_MANIFEST" ]] || {
    echo "missing expected official source manifest: $FFT_OFFICIAL_EXPECTED_SOURCE_MANIFEST" >&2
    exit 7
  }
  if ! cmp -s "$resolved_root/manifests/source.sha256" "$FFT_OFFICIAL_EXPECTED_SOURCE_MANIFEST"; then
    echo "source manifest drift detected during official run: $resolved_root" >&2
    exit 7
  fi
  printf '%s\n' "$FFT_OFFICIAL_EXPECTED_SOURCE_MANIFEST" \
    > "$resolved_root/manifests/expected-source-manifest.txt"
fi

sha256sum "$XS_ROOT/nexus-am/am/build/am-riscv64-xs.a" "$XS_ROOT/nexus-am/libs/klib/build/klib-riscv64-xs.a" > "$resolved_root/manifests/shared-archives-before.sha256"
sha256sum "$EMU_BIN" > "$resolved_root/manifests/emu.sha256"

python3 "$SCRIPT_DIR/generate_fft_corpus.py" \
  --corpus "$CORPUS_ID" --points "$FFT_POINTS" \
  --output "$resolved_root/corpus/data" --overlay "$resolved_root/overlay" \
  > "$resolved_root/corpus/generator.json"

build_base="$resolved_root/build/fft-${DFT_IMPL}-n${FFT_POINTS}-idft${FFT_RUN_IDFT}"
binary="$build_base.bin"
printf '%s\n' \
  "cd $XS_ROOT" \
  "source ./env.sh" \
  "make -C $APP_DIR -o am -o klib -o prompt ARCH=riscv64-xs DFT_IMPL=$DFT_IMPL FFT_EVIDENCE_ONLY=1 FFT_POINTS=$FFT_POINTS FFT_REPEAT=$repeat FFT_WARMUP_REPEAT=$warmup FFT_RUN_IDFT=$FFT_RUN_IDFT FFT_VALIDATE=0 FFT_ROI_HPM_ENABLE=1 FFT_ROI_HPM_PROFILE=canonical CPPFLAGS=-I${resolved_root}/overlay\\ ${EXTRA_CPPFLAGS} DST_DIR=${resolved_root}/build/objects/ BINARY=${build_base} -j1" \
  > "$resolved_root/manifests/build-command.txt"

(
  cd "$XS_ROOT"
  source ./env.sh >/dev/null
  make -C "$APP_DIR" -o am -o klib -o prompt \
    ARCH=riscv64-xs DFT_IMPL="$DFT_IMPL" FFT_EVIDENCE_ONLY=1 \
    FFT_POINTS="$FFT_POINTS" FFT_REPEAT="$repeat" FFT_WARMUP_REPEAT="$warmup" \
    FFT_RUN_IDFT="$FFT_RUN_IDFT" FFT_VALIDATE=0 FFT_ROI_HPM_ENABLE=1 \
    FFT_ROI_HPM_PROFILE=canonical \
    CPPFLAGS="-I${resolved_root}/overlay ${EXTRA_CPPFLAGS}" \
    DST_DIR="${resolved_root}/build/objects/" BINARY="$build_base" -j1
) > "$resolved_root/logs/build.log" 2>&1

for artifact in "$binary" "$build_base.elf" "$build_base.txt"; do
  [[ -f "$artifact" ]] || { echo "missing build artifact: $artifact" >&2; exit 5; }
done
object_manifest="$resolved_root/manifests/objects.sha256"
find "$resolved_root/build/objects" -type f -name '*.o' -print0 \
  | sort -z | xargs -0 -r sha256sum > "$object_manifest"
sha256sum "$binary" "$build_base.elf" "$build_base.txt" > "$resolved_root/manifests/binary.sha256"
if [[ -s "$object_manifest" ]]; then
  cat "$object_manifest" >> "$resolved_root/manifests/binary.sha256"
fi
sha256sum "$XS_ROOT/nexus-am/am/build/am-riscv64-xs.a" "$XS_ROOT/nexus-am/libs/klib/build/klib-riscv64-xs.a" > "$resolved_root/manifests/shared-archives-after.sha256"
cmp -s "$resolved_root/manifests/shared-archives-before.sha256" "$resolved_root/manifests/shared-archives-after.sha256" || {
  echo "shared AM/klib archive changed" >&2
  exit 6
}

report="$resolved_root/official/emu.report.txt"
set +e
(
  cd "$XS_ROOT"
  source ./env.sh >/dev/null
  "$EMU_BIN" --no-diff --force-dump-result -i "$binary"
) > "$report" 2>&1
emu_rc=$?
set -e
printf '%s\n' "$emu_rc" > "$resolved_root/official/emu.exit_code"

python3 "$SCRIPT_DIR/parse_fft_emu_report.py" \
  --report "$report" --output "$resolved_root/official/result.json" \
  --binary "$binary" --source-manifest "$resolved_root/manifests/source.sha256" \
  --corpus-manifest "$resolved_root/corpus/data/corpus.json" \
  --impl "$DFT_IMPL" --points "$FFT_POINTS" --idft "$FFT_RUN_IDFT" \
  --corpus "$CORPUS_ID" --warmup "$warmup" --repeat "$repeat" --emu-exit "$emu_rc"

printf '%s\n' \
  "impl=$DFT_IMPL" "points=$FFT_POINTS" "idft=$FFT_RUN_IDFT" "corpus=$CORPUS_ID" \
  "warmup=$warmup" "repeat=$repeat" "timeout=0" "official=1" \
  "emu=$EMU_BIN" "extra_cppflags=$EXTRA_CPPFLAGS" "output_root=$resolved_evidence" \
  > "$resolved_root/manifests/run.env"
