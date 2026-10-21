#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
EVIDENCE_ROOT="${APP_DIR}/evidence"

usage() {
  printf 'usage: %s CANDIDATE_ID RUN_ID\n' "$0" >&2
  exit 2
}
[[ $# -eq 2 ]] || usage
CANDIDATE_ID="$1"
RUN_ID="$2"

case "$CANDIDATE_ID" in
  FFT-EXP-*) ;;
  *) echo "invalid candidate id: ${CANDIDATE_ID}" >&2; exit 2 ;;
esac
case "$RUN_ID" in
  ''|*[!A-Za-z0-9._-]*) echo "invalid run id: ${RUN_ID}" >&2; exit 2 ;;
esac

ROOT="$(realpath -m -- "${EVIDENCE_ROOT}/${CANDIDATE_ID}/${RUN_ID}")"
RESOLVED_EVIDENCE="$(realpath -m -- "$EVIDENCE_ROOT")"
case "$ROOT/" in
  "$RESOLVED_EVIDENCE"/*) ;;
  *) echo "refusing output outside evidence: ${ROOT}" >&2; exit 3 ;;
esac
if [[ -e "$ROOT" ]]; then
  if [[ ! -d "$ROOT/state" ]]; then
    echo "refusing to overwrite matrix root: ${ROOT}" >&2
    exit 3
  fi
  if find "$ROOT" -mindepth 1 -maxdepth 1 ! -name state -print -quit | grep -q .; then
    echo "refusing non-empty matrix root: ${ROOT}" >&2
    exit 3
  fi
else
  mkdir -p "$ROOT"
fi
mkdir -p "$ROOT/state" "$ROOT/runs" "$ROOT/logs" "$ROOT/manifests"

start_epoch="$(date +%s)"
for state_file in pid start_time exit_code end_time wall_seconds status; do
  if [[ -e "$ROOT/state/$state_file" ]]; then
    echo "refusing to overwrite state file: ${ROOT}/state/${state_file}" >&2
    exit 3
  fi
done
printf '%s\n' "$$" > "$ROOT/state/pid"
printf '%s\n' "$(date --iso-8601=seconds)" > "$ROOT/state/start_time"

finish() {
  rc=$?
  printf '%s\n' "$rc" > "$ROOT/state/exit_code"
  printf '%s\n' "$(date --iso-8601=seconds)" > "$ROOT/state/end_time"
  printf '%s\n' "$(( $(date +%s) - start_epoch ))" > "$ROOT/state/wall_seconds"
  if [[ "$rc" -eq 0 ]]; then
    printf 'PASS\n' > "$ROOT/state/status"
  else
    printf 'FAIL\n' > "$ROOT/state/status"
  fi
}
trap finish EXIT

source_manifest="$ROOT/manifests/source.sha256"
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
) > "$source_manifest"
export FFT_PREFLIGHT_EXPECTED_SOURCE_MANIFEST="$source_manifest"

printf 'impl\tpoints\tidft\tcorpus\tchild_root\trc\tstatus\n' > "$ROOT/matrix.tsv"
printf '%s\n' \
  "candidate_id=${CANDIDATE_ID}" \
  "run_id=${RUN_ID}" \
  "source_manifest=${source_manifest}" \
  "source_manifest_guard=enabled" \
  "corpora=${FFT_PREFLIGHT_CORPORA:-all}" \
  "impls=${FFT_PREFLIGHT_IMPLS:-all}" \
  "points=${FFT_PREFLIGHT_POINTS:-all}" \
  "idfts=${FFT_PREFLIGHT_IDFTS:-0,1}" \
  > "$ROOT/manifests/matrix.env"

IFS=',' read -r -a corpora <<< "${FFT_PREFLIGHT_CORPORA:-C0-legacy-pattern-v1,C1-impulse-v1,C2-dc-alternating-v1,C3-prng-q15-v1,C4-near-limit-v1}"
IFS=',' read -r -a impls <<< "${FFT_PREFLIGHT_IMPLS:-dft_scalar,dft_rvv,dft_ext}"
IFS=',' read -r -a points_list <<< "${FFT_PREFLIGHT_POINTS:-16,64,128,256,512,1024,2048,4096}"
IFS=',' read -r -a idfts <<< "${FFT_PREFLIGHT_IDFTS:-0,1}"

failures=0
for corpus in "${corpora[@]}"; do
  for impl in "${impls[@]}"; do
    for points in "${points_list[@]}"; do
      for idft in "${idfts[@]}"; do
        child_name="${impl}-n${points}-idft${idft}-${corpus}"
        child_root="$ROOT/runs/$child_name"
        printf '[%s] START %s\n' "$(date --iso-8601=seconds)" "$child_name"
        set +e
        bash "$SCRIPT_DIR/run_fft_preflight.sh" \
          "$child_root" "$impl" "$points" "$idft" "$corpus"
        rc=$?
        set -e
        child_status=FAIL
        if [[ "$rc" -eq 0 ]]; then
          child_status=PASS
        else
          failures=$((failures + 1))
        fi
        printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\n' \
          "$impl" "$points" "$idft" "$corpus" "$child_root" "$rc" "$child_status" \
          >> "$ROOT/matrix.tsv"
        printf '[%s] DONE %s rc=%s status=%s\n' \
          "$(date --iso-8601=seconds)" "$child_name" "$rc" "$child_status"
      done
    done
  done
done

printf 'failures=%s\n' "$failures" > "$ROOT/manifests/summary.env"
if [[ "$failures" -ne 0 ]]; then
  echo "preflight matrix completed with ${failures} failed workloads" >&2
  exit 1
fi
echo "preflight matrix completed successfully"
