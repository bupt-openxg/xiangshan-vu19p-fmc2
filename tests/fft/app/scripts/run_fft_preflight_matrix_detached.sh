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
  echo "refusing to overwrite matrix root: ${ROOT}" >&2
  exit 3
fi

mkdir -p "$ROOT/state"
launcher_log="$ROOT/state/launcher.log"
printf '%s\n' \
  "candidate_id=${CANDIDATE_ID}" \
  "run_id=${RUN_ID}" \
  "artifact_root=${ROOT}" \
  "command=${SCRIPT_DIR}/run_fft_preflight_matrix.sh ${CANDIDATE_ID} ${RUN_ID}" \
  > "$ROOT/state/launcher.env"

nohup setsid bash "$SCRIPT_DIR/run_fft_preflight_matrix.sh" \
  "$CANDIDATE_ID" "$RUN_ID" \
  > "$launcher_log" 2>&1 < /dev/null &
launcher_pid=$!
printf '%s\n' "$launcher_pid" > "$ROOT/state/launcher_pid"
printf 'artifact_root=%s\nlauncher_pid=%s\n' "$ROOT" "$launcher_pid"
