#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
EVIDENCE_ROOT="${APP_DIR}/evidence"

usage() {
  printf 'usage: %s CANDIDATE_ID RUN_ID IMPL POINTS IDFT CORPUS_ID\n' "$0" >&2
  exit 2
}
[[ $# -eq 6 ]] || usage
CANDIDATE_ID="$1"
RUN_ID="$2"
DFT_IMPL="$3"
FFT_POINTS="$4"
FFT_RUN_IDFT="$5"
CORPUS_ID="$6"

case "$CANDIDATE_ID" in
  FFT-EXP-*) ;;
  *) echo "invalid candidate id: ${CANDIDATE_ID}" >&2; exit 2 ;;
esac
case "$RUN_ID" in
  ''|*[!A-Za-z0-9._-]*) echo "invalid run id: ${RUN_ID}" >&2; exit 2 ;;
esac

artifact_root="${EVIDENCE_ROOT}/${CANDIDATE_ID}/${RUN_ID}"
resolved_root="$(realpath -m -- "$artifact_root")"
resolved_evidence="$(realpath -m -- "$EVIDENCE_ROOT")"
case "$resolved_root/" in
  "$resolved_evidence"/*) ;;
  *) echo "refusing output outside evidence: ${resolved_root}" >&2; exit 3 ;;
esac
if [[ -e "$resolved_root" ]]; then
  echo "refusing to overwrite existing artifact root: ${resolved_root}" >&2
  exit 3
fi

mkdir -p "$resolved_root/state"
launcher_log="$resolved_root/state/launcher.log"
printf '%s\n' \
  "candidate_id=${CANDIDATE_ID}" \
  "run_id=${RUN_ID}" \
  "artifact_root=${resolved_root}" \
  "command=${SCRIPT_DIR}/run_fft_preflight.sh ${resolved_root} ${DFT_IMPL} ${FFT_POINTS} ${FFT_RUN_IDFT} ${CORPUS_ID}" \
  > "$resolved_root/state/launcher.env"

nohup setsid bash "$SCRIPT_DIR/run_fft_preflight.sh" \
  "$resolved_root" "$DFT_IMPL" "$FFT_POINTS" "$FFT_RUN_IDFT" "$CORPUS_ID" \
  > "$launcher_log" 2>&1 < /dev/null &
launcher_pid=$!
printf '%s\n' "$launcher_pid" > "$resolved_root/state/launcher_pid"
printf 'artifact_root=%s\nlauncher_pid=%s\n' "$resolved_root" "$launcher_pid"
