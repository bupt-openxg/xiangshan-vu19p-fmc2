#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
APP=${ROOT}/nexus-am/apps/mimo

usage() {
  cat <<'EOF'
Usage:
  scripts/compare_emu_roi.sh <baseline_log> <candidate_log>

Both logs must contain MIMO_ROI. Paths may be absolute or relative to the
small-letter mimo project root.
EOF
}

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
  usage
  exit 0
fi

if [[ $# -ne 2 ]]; then
  usage >&2
  exit 2
fi

resolve_log() {
  local path=$1
  if [[ "${path}" == /* ]]; then
    printf '%s\n' "${path}"
  else
    printf '%s/%s\n' "${APP}" "${path}"
  fi
}

summary_value() {
  local file=$1
  local key=$2
  awk -F '=' -v k="${key}" '$1 == k { print $2; found = 1; exit } END { if (!found) exit 1 }' "${file}"
}

baseline_log=$(resolve_log "$1")
candidate_log=$(resolve_log "$2")

if [[ ! -f "${baseline_log}" ]]; then
  echo "Missing baseline log: ${baseline_log}" >&2
  exit 1
fi
if [[ ! -f "${candidate_log}" ]]; then
  echo "Missing candidate log: ${candidate_log}" >&2
  exit 1
fi

tmp_base=$(mktemp)
tmp_candidate=$(mktemp)
trap 'rm -f "${tmp_base}" "${tmp_candidate}"' EXIT

"${APP}/scripts/summarize_emu_results.sh" --log "${baseline_log}" > "${tmp_base}"
"${APP}/scripts/summarize_emu_results.sh" --log "${candidate_log}" > "${tmp_candidate}"

base_roi=$(summary_value "${tmp_base}" roi_present || true)
candidate_roi=$(summary_value "${tmp_candidate}" roi_present || true)

if [[ "${base_roi}" != "1" ]]; then
  echo "Baseline ROI missing: ${baseline_log}" >&2
  cat "${tmp_base}" >&2
  exit 3
fi
if [[ "${candidate_roi}" != "1" ]]; then
  echo "Candidate ROI missing: ${candidate_log}" >&2
  cat "${tmp_candidate}" >&2
  exit 3
fi

base_cycle=$(summary_value "${tmp_base}" roi_cycle)
base_instret=$(summary_value "${tmp_base}" roi_instret)
base_ipc=$(summary_value "${tmp_base}" roi_ipc)
base_backend=$(summary_value "${tmp_base}" roi_backend)
base_kernel=$(summary_value "${tmp_base}" roi_kernel)

candidate_cycle=$(summary_value "${tmp_candidate}" roi_cycle)
candidate_instret=$(summary_value "${tmp_candidate}" roi_instret)
candidate_ipc=$(summary_value "${tmp_candidate}" roi_ipc)
candidate_backend=$(summary_value "${tmp_candidate}" roi_backend)
candidate_kernel=$(summary_value "${tmp_candidate}" roi_kernel)

base_wall=$(summary_value "${tmp_base}" runtime_elapsed_sec || echo "missing")
candidate_wall=$(summary_value "${tmp_candidate}" runtime_elapsed_sec || echo "missing")
base_host_ms=$(summary_value "${tmp_base}" host_time_ms || echo "missing")
candidate_host_ms=$(summary_value "${tmp_candidate}" host_time_ms || echo "missing")

awk \
  -v base_log="${baseline_log}" \
  -v candidate_log="${candidate_log}" \
  -v base_backend="${base_backend}" \
  -v candidate_backend="${candidate_backend}" \
  -v base_kernel="${base_kernel}" \
  -v candidate_kernel="${candidate_kernel}" \
  -v base_cycle="${base_cycle}" \
  -v candidate_cycle="${candidate_cycle}" \
  -v base_instret="${base_instret}" \
  -v candidate_instret="${candidate_instret}" \
  -v base_ipc="${base_ipc}" \
  -v candidate_ipc="${candidate_ipc}" \
  -v base_wall="${base_wall}" \
  -v candidate_wall="${candidate_wall}" \
  -v base_host_ms="${base_host_ms}" \
  -v candidate_host_ms="${candidate_host_ms}" \
  'BEGIN {
    printf("baseline_log=%s\n", base_log);
    printf("candidate_log=%s\n", candidate_log);
    printf("baseline=%s/%s\n", base_backend, base_kernel);
    printf("candidate=%s/%s\n", candidate_backend, candidate_kernel);
    printf("baseline_cycle=%s\n", base_cycle);
    printf("candidate_cycle=%s\n", candidate_cycle);
    printf("cycle_ratio_candidate_over_baseline=%.6f\n", candidate_cycle / base_cycle);
    printf("cycle_delta=%d\n", candidate_cycle - base_cycle);
    printf("baseline_instret=%s\n", base_instret);
    printf("candidate_instret=%s\n", candidate_instret);
    printf("instret_ratio_candidate_over_baseline=%.6f\n", candidate_instret / base_instret);
    printf("instret_delta=%d\n", candidate_instret - base_instret);
    printf("baseline_roi_ipc=%s\n", base_ipc);
    printf("candidate_roi_ipc=%s\n", candidate_ipc);
    printf("baseline_wall_sec=%s\n", base_wall);
    printf("candidate_wall_sec=%s\n", candidate_wall);
    printf("baseline_host_ms=%s\n", base_host_ms);
    printf("candidate_host_ms=%s\n", candidate_host_ms);
  }'
