#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
EVIDENCE_ROOT="${APP_DIR}/evidence"

usage() { printf 'usage: %s CANDIDATE_ID RUN_ID\n' "$0" >&2; exit 2; }
[[ $# -eq 2 ]] || usage
CANDIDATE_ID="$1"
RUN_ID="$2"
case "$CANDIDATE_ID" in FFT-EXP-*) ;; *) echo "invalid candidate id: $CANDIDATE_ID" >&2; exit 2 ;; esac
case "$RUN_ID" in ''|*[!A-Za-z0-9._-]*) echo "invalid run id: $RUN_ID" >&2; exit 2 ;; esac

ROOT="$(realpath -m -- "${EVIDENCE_ROOT}/${CANDIDATE_ID}/${RUN_ID}")"
EVIDENCE="$(realpath -m -- "$EVIDENCE_ROOT")"
case "$ROOT/" in "$EVIDENCE"/*) ;; *) echo "refusing output outside evidence: $ROOT" >&2; exit 3 ;; esac
if [[ -e "$ROOT" ]]; then
  [[ -d "$ROOT/state" ]] || { echo "refusing to overwrite matrix root: $ROOT" >&2; exit 3; }
  if find "$ROOT" -mindepth 1 -maxdepth 1 ! -name state -print -quit | grep -q .; then
    echo "refusing non-empty matrix root: $ROOT" >&2
    exit 3
  fi
else
  mkdir -p "$ROOT"
fi
mkdir -p "$ROOT/state" "$ROOT/runs" "$ROOT/official" "$ROOT/manifests" "$ROOT/logs"
for f in pid start_time exit_code end_time wall_seconds status; do
  [[ ! -e "$ROOT/state/$f" ]] || { echo "refusing state overwrite: $ROOT/state/$f" >&2; exit 3; }
done

start_epoch="$(date +%s)"
printf '%s\n' "$$" > "$ROOT/state/pid"
printf '%s\n' "$(date --iso-8601=seconds)" > "$ROOT/state/start_time"
finish() {
  rc=$?
  printf '%s\n' "$rc" > "$ROOT/state/exit_code"
  printf '%s\n' "$(date --iso-8601=seconds)" > "$ROOT/state/end_time"
  printf '%s\n' "$(( $(date +%s) - start_epoch ))" > "$ROOT/state/wall_seconds"
  if [[ "$rc" -eq 0 ]]; then printf 'PASS\n' > "$ROOT/state/status"; else printf 'FAIL\n' > "$ROOT/state/status"; fi
}
trap finish EXIT

(
  cd "$APP_DIR"
  find . -type f \
    ! -path './build/*' ! -path './evidence/*' ! -path './archive/*' \
    ! -path './docs/*' ! -path './.vscode/*' ! -path './scripts/__pycache__/*' \
    -print0 | sort -z | xargs -0 sha256sum
) > "$ROOT/manifests/source.sha256"
export FFT_OFFICIAL_EXPECTED_SOURCE_MANIFEST="$ROOT/manifests/source.sha256"
printf 'impl\tpoints\tidft\tcorpus\tchild_root\trc\tstatus\tcycle\tinstret\n' > "$ROOT/official/matrix.tsv"
printf '%s\n' \
  "candidate_id=$CANDIDATE_ID" "run_id=$RUN_ID" \
  "warmup/repeat=canonical-by-point" "timeout=0" \
  "source_manifest_guard=enabled" \
  "extra_cppflags=${FFT_OFFICIAL_EXTRA_CPPFLAGS:-}" > "$ROOT/manifests/matrix.env"

IFS=',' read -r -a impls <<< "${FFT_OFFICIAL_IMPLS:-dft_scalar,dft_rvv,dft_ext}"
IFS=',' read -r -a points_list <<< "${FFT_OFFICIAL_POINTS:-16,64,128,256,512,1024,2048,4096}"
IFS=',' read -r -a idfts <<< "${FFT_OFFICIAL_IDFTS:-0,1}"
corpus=C0-legacy-pattern-v1
failures=0
for impl in "${impls[@]}"; do
  for points in "${points_list[@]}"; do
    for idft in "${idfts[@]}"; do
      child_name="${impl}-n${points}-idft${idft}-${corpus}"
      child_root="$ROOT/runs/$child_name"
      printf '[%s] START %s\n' "$(date --iso-8601=seconds)" "$child_name"
      set +e
      bash "$SCRIPT_DIR/run_fft_official.sh" "$child_root" "$impl" "$points" "$idft" "$corpus"
      rc=$?
      set -e
      status=FAIL
      cycle=NA
      instret=NA
      if [[ "$rc" -eq 0 ]]; then
        status=PASS
        cycle="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["roi"]["cycle"])' "$child_root/official/result.json")"
        instret="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["roi"]["instret"])' "$child_root/official/result.json")"
      else
        failures=$((failures + 1))
      fi
      printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' \
        "$impl" "$points" "$idft" "$corpus" "$child_root" "$rc" "$status" "$cycle" "$instret" \
        >> "$ROOT/official/matrix.tsv"
      printf '[%s] DONE %s rc=%s status=%s\n' "$(date --iso-8601=seconds)" "$child_name" "$rc" "$status"
    done
  done
done

printf 'failures=%s\n' "$failures" > "$ROOT/manifests/summary.env"
if [[ "$failures" -ne 0 ]]; then
  echo "official matrix completed with $failures failures" >&2
  exit 1
fi
echo "official matrix completed successfully"
