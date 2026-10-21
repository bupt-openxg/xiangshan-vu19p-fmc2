#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
APP=${ROOT}/nexus-am/apps/mimo

usage() {
  cat <<'EOF'
Usage:
  scripts/extract_emu_perf_counters.sh --log <log_path> [regex ...]
  scripts/extract_emu_perf_counters.sh --all --log <log_path>

Paths may be absolute or relative to the small-letter mimo project root.
Without regex arguments, a conservative default set of frontend/backend
summary counters is printed.
EOF
}

all=0
log_path=
patterns=()

while [[ $# -gt 0 ]]; do
  case "$1" in
    -h|--help)
      usage
      exit 0
      ;;
    --all)
      all=1
      shift
      ;;
    --log)
      log_path=${2:-}
      shift 2
      ;;
    *)
      patterns+=("$1")
      shift
      ;;
  esac
done

if [[ -z "${log_path}" ]]; then
  usage >&2
  exit 2
fi

if [[ "${log_path}" != /* ]]; then
  log_path="${APP}/${log_path}"
fi

if [[ ! -f "${log_path}" ]]; then
  echo "Missing log: ${log_path}" >&2
  exit 1
fi

if [[ ${all} -eq 0 && ${#patterns[@]} -eq 0 ]]; then
  patterns=(
    'clock_cycle$'
    'total_commits$'
    'validCycles$'
    'validInstrs$'
    'stallCycles_fetch_ftqNotvalid$'
    'commit_branch_num$'
    'commit_branch_mispredicts$'
    'csr_instr_cnt$'
    'wait_cycles?$'
    'valid_cnt_hist_.*_sum$'
    'ready_cnt_hist_.*_sum$'
    'issue_datasource_.*_sum$'
  )
fi

pattern_file=$(mktemp)
trap 'rm -f "${pattern_file}"' EXIT

if [[ ${all} -eq 0 ]]; then
  printf '%s\n' "${patterns[@]}" > "${pattern_file}"
fi

awk -v all="${all}" -v pattern_file="${pattern_file}" '
  BEGIN {
    if (!all) {
      while ((getline p < pattern_file) > 0) {
        if (p != "") {
          patterns[++pattern_count] = p;
        }
      }
      close(pattern_file);
    }
    print "perf_key\tvalue";
  }

  function wanted(key,    i) {
    if (all) {
      return 1;
    }
    for (i = 1; i <= pattern_count; ++i) {
      if (key ~ patterns[i]) {
        return 1;
      }
    }
    return 0;
  }

  /\[PERF \]/ {
    line = $0;
    gsub(/\r/, "", line);
    gsub(/\033\[[0-9;]*[A-Za-z]/, "", line);

    sub(/^.*\] /, "", line);
    key = line;
    value = line;
    sub(/,[[:space:]]*[0-9]+[[:space:]]*$/, "", key);
    sub(/^.*,[[:space:]]*/, "", value);

    if (key != value && wanted(key)) {
      print key "\t" value;
    }
  }
' "${log_path}"
