#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
APP=${ROOT}/nexus-am/apps/mimo
runtime_tsv="${APP}/perf_eval/raw/emu/emu_runtime_observations.tsv"

usage() {
  cat <<'EOF'
Usage:
  scripts/summarize_emu_results.sh <backend> <kernel>
  scripts/summarize_emu_results.sh --best <backend> <kernel>
  scripts/summarize_emu_results.sh --log <log_path>

Examples:
  scripts/summarize_emu_results.sh scalar eq_zf_4x4
  scripts/summarize_emu_results.sh --best ext eq_zf_4x4
  scripts/summarize_emu_results.sh --log perf_eval/raw/emu/emu-mimo-rvv-eq_zf_4x4-w0-r1-20260619-200707.log
EOF
}

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
  usage
  exit 0
fi

log_path=
backend=${1:-}
kernel=${2:-}
selection=latest
best_candidates=0

if [[ "${1:-}" == "--log" ]]; then
  log_path=${2:-}
  backend=
  kernel=
  selection=explicit-log
elif [[ "${1:-}" == "--best" ]]; then
  backend=${2:-}
  kernel=${3:-}
  selection=best-cycle
  if [[ -z "${backend}" || -z "${kernel}" ]]; then
    usage >&2
    exit 2
  fi
elif [[ -z "${backend}" || -z "${kernel}" ]]; then
  usage >&2
  exit 2
fi

if [[ "${selection}" == "best-cycle" ]]; then
  best_cycle=
  best_log=

  while IFS= read -r candidate; do
    grep -q 'HIT GOOD TRAP' "${candidate}" || continue

    roi_line=$(grep -m 1 'MIMO_ROI ' "${candidate}" | tr -d '\r' || true)
    [[ -n "${roi_line}" ]] || continue

    parsed_roi=$(awk '
      {
        for (i = 1; i <= NF; ++i) {
          split($i, kv, "=");
          if (kv[1] == "kernel") kernel = kv[2];
          if (kv[1] == "backend") backend = kv[2];
          if (kv[1] == "cycle") cycle = kv[2];
        }
        printf("%s %s %s\n", backend, kernel, cycle);
      }
    ' <<< "${roi_line}")
    read -r roi_backend roi_kernel roi_cycle <<< "${parsed_roi}"
    [[ "${roi_backend}" == "${backend}" && "${roi_kernel}" == "${kernel}" ]] || continue
    [[ "${roi_cycle}" =~ ^[0-9]+$ ]] || continue

    done_line=$(grep -m 1 'MIMO_DONE ' "${candidate}" | tr -d '\r' || true)
    done_status=$(awk '
      {
        for (i = 1; i <= NF; ++i) {
          split($i, kv, "=");
          if (kv[1] == "status") status = kv[2];
        }
        print status;
      }
    ' <<< "${done_line}")
    [[ "${done_status}" == "0" ]] || continue

    best_candidates=$((best_candidates + 1))
    if [[ -z "${best_cycle}" || "${roi_cycle}" -lt "${best_cycle}" ]]; then
      best_cycle=${roi_cycle}
      best_log=${candidate}
    fi
  done < <(find "${APP}/perf_eval/raw/emu" -maxdepth 1 -type f \
    -name "emu-mimo-${backend}-${kernel}-w*-r*-*.log" | sort)

  log_path=${best_log}
elif [[ -n "${log_path}" ]]; then
  if [[ "${log_path}" != /* ]]; then
    log_path="${APP}/${log_path}"
  fi
else
  if [[ -f "${runtime_tsv}" ]]; then
    log_path=$(awk -F '\t' -v b="${backend}" -v k="${kernel}" '
      NR > 1 && $6 == b && $7 == k { out = $10 }
      END { print out }
    ' "${runtime_tsv}")
  fi
  if [[ -z "${log_path}" ]]; then
    log_path=$(ls -t "${APP}/perf_eval/raw/emu/emu-mimo-${backend}-${kernel}"*.log 2>/dev/null | head -n 1 || true)
  fi
fi

if [[ -z "${log_path}" || ! -f "${log_path}" ]]; then
  echo "summary_status=missing_log"
  echo "log=${log_path}"
  exit 1
fi

strip_ansi() {
  sed -E 's/\x1B\[[0-9;]*[A-Za-z]//g'
}

roi_line=$(grep -m 1 'MIMO_ROI ' "${log_path}" | tr -d '\r' || true)
done_line=$(grep -m 1 'MIMO_DONE ' "${log_path}" | tr -d '\r' || true)
program_line=$(grep -m 1 'instrCnt = .*cycleCnt = .*IPC =' "${log_path}" | tr -d '\r' | strip_ansi || true)
host_line=$(grep -m 1 'Host time spent:' "${log_path}" | tr -d '\r' | strip_ansi || true)

runtime_line=
if [[ -f "${runtime_tsv}" ]]; then
  runtime_line=$(awk -F '\t' -v target_log="${log_path}" '
    NR > 1 && $10 == target_log { out = $0 }
    END { print out }
  ' "${runtime_tsv}")
fi

echo "summary_status=ok"
echo "selection=${selection}"
if [[ "${selection}" == "best-cycle" ]]; then
  echo "best_candidates=${best_candidates}"
fi
echo "log=${log_path}"

if [[ -n "${runtime_line}" ]]; then
  awk -F '\t' '
    {
      printf("runtime_start_iso=%s\n", $1);
      printf("runtime_end_iso=%s\n", $2);
      printf("runtime_elapsed_sec=%s\n", $3);
      printf("runtime_status=%s\n", $4);
      printf("runtime_timeout_sec=%s\n", $5);
      printf("runtime_backend=%s\n", $6);
      printf("runtime_kernel=%s\n", $7);
      printf("runtime_warmup=%s\n", $8);
      printf("runtime_repeat=%s\n", $9);
    }
  ' <<< "${runtime_line}"
else
  echo "runtime_record=missing"
fi

if [[ -n "${roi_line}" ]]; then
  awk '
    {
      for (i = 1; i <= NF; ++i) {
        split($i, kv, "=");
        if (kv[1] == "kernel") kernel = kv[2];
        if (kv[1] == "backend") backend = kv[2];
        if (kv[1] == "cycle") cycle = kv[2];
        if (kv[1] == "instret") instret = kv[2];
        if (kv[1] == "repeat") repeat = kv[2];
      }
      printf("roi_present=1\n");
      printf("roi_kernel=%s\n", kernel);
      printf("roi_backend=%s\n", backend);
      printf("roi_cycle=%s\n", cycle);
      printf("roi_instret=%s\n", instret);
      printf("roi_repeat=%s\n", repeat);
      if (cycle > 0) {
        printf("roi_ipc=%.6f\n", instret / cycle);
      }
    }
  ' <<< "${roi_line}"
else
  echo "roi_present=0"
fi

if [[ -n "${done_line}" ]]; then
  awk '
    {
      for (i = 1; i <= NF; ++i) {
        split($i, kv, "=");
        if (kv[1] == "status") status = kv[2];
        if (kv[1] == "checksum") checksum = kv[2];
      }
      printf("done_present=1\n");
      printf("done_status=%s\n", status);
      printf("done_checksum=%s\n", checksum);
    }
  ' <<< "${done_line}"
else
  echo "done_present=0"
fi

if grep -q 'HIT GOOD TRAP' "${log_path}"; then
  echo "trap=good"
elif grep -q 'BAD TRAP' "${log_path}"; then
  echo "trap=bad"
else
  echo "trap=missing"
fi

if [[ -n "${program_line}" ]]; then
  sed -E 's/.*instrCnt = ([0-9]+), cycleCnt = ([0-9]+), IPC = ([0-9.]+).*/program_instrCnt=\1\nprogram_cycleCnt=\2\nprogram_ipc=\3/' <<< "${program_line}"
fi

if [[ -n "${host_line}" ]]; then
  sed -E 's/.*Host time spent: ([0-9]+)ms.*/host_time_ms=\1/' <<< "${host_line}"
fi
