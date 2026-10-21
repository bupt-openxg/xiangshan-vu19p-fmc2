#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
EVIDENCE_ROOT="${APP_DIR}/evidence"
[[ $# -eq 2 ]] || { printf 'usage: %s CANDIDATE_ID RUN_ID\n' "$0" >&2; exit 2; }
CANDIDATE_ID="$1"
RUN_ID="$2"
case "$CANDIDATE_ID" in FFT-EXP-*) ;; *) echo "invalid candidate id: $CANDIDATE_ID" >&2; exit 2 ;; esac
case "$RUN_ID" in ''|*[!A-Za-z0-9._-]*) echo "invalid run id: $RUN_ID" >&2; exit 2 ;; esac
ROOT="$(realpath -m -- "${EVIDENCE_ROOT}/${CANDIDATE_ID}/${RUN_ID}")"
EVIDENCE="$(realpath -m -- "$EVIDENCE_ROOT")"
case "$ROOT/" in "$EVIDENCE"/*) ;; *) echo "refusing output outside evidence: $ROOT" >&2; exit 3 ;; esac
[[ ! -e "$ROOT" ]] || { echo "refusing to overwrite matrix root: $ROOT" >&2; exit 3; }
mkdir -p "$ROOT/state"
printf '%s\n' \
  "candidate_id=$CANDIDATE_ID" "run_id=$RUN_ID" "artifact_root=$ROOT" \
  "command=$SCRIPT_DIR/run_fft_official_matrix.sh $CANDIDATE_ID $RUN_ID" \
  > "$ROOT/state/launcher.env"
nohup setsid bash "$SCRIPT_DIR/run_fft_official_matrix.sh" "$CANDIDATE_ID" "$RUN_ID" \
  > "$ROOT/state/launcher.log" 2>&1 < /dev/null &
pid=$!
printf '%s\n' "$pid" > "$ROOT/state/launcher_pid"
printf 'artifact_root=%s\nlauncher_pid=%s\n' "$ROOT" "$pid"
