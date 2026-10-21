#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
APP=${ROOT}/nexus-am/apps/mimo

usage() {
  cat <<'EOF'
Usage:
  scripts/audit_completed_emu_result.sh --log <log_path> [options]

Options:
  --expected-checksum <value>     Expected MIMO_DONE checksum. Default: 945992261.
  --snapshot-manifest <path>      Optional job build snapshot manifest to verify.
  --expect-manifest <key=value>   Require a key=value line in snapshot manifest.
                                  May be repeated.
  --baseline <name>=<log_path>    Compare candidate ROI against a completed baseline.
                                  May be repeated.

The script is read-only. It refuses incomplete logs: runtime record, ROI,
MIMO_DONE status=0, expected checksum, and HIT GOOD TRAP must all be present.
If a snapshot manifest contains run_image=, the script also checks that it
points to snapshot_prefix.bin and that the file exists. Old manifests without
run_image= remain valid.
EOF
}

log_path=
expected_checksum=945992261
snapshot_manifest=
manifest_expectations=()
baselines=()

while [[ $# -gt 0 ]]; do
  case "$1" in
    -h|--help)
      usage
      exit 0
      ;;
    --log)
      log_path=${2:-}
      shift 2
      ;;
    --expected-checksum)
      expected_checksum=${2:-}
      shift 2
      ;;
    --snapshot-manifest)
      snapshot_manifest=${2:-}
      shift 2
      ;;
    --expect-manifest)
      manifest_expectations+=("${2:-}")
      shift 2
      ;;
    --baseline)
      baselines+=("${2:-}")
      shift 2
      ;;
    *)
      usage >&2
      exit 2
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

if [[ -n "${snapshot_manifest}" && "${snapshot_manifest}" != /* ]]; then
  snapshot_manifest="${APP}/${snapshot_manifest}"
fi

if [[ ! -f "${log_path}" ]]; then
  echo "audit_status=missing_log"
  echo "log=${log_path}"
  exit 1
fi

summary_file=$(mktemp)
trap 'rm -f "${summary_file}"' EXIT

"${APP}/scripts/summarize_emu_results.sh" --log "${log_path}" > "${summary_file}"

summary_value() {
  local key=$1
  awk -F '=' -v k="${key}" '$1 == k { print $2; found = 1; exit } END { if (!found) exit 1 }' "${summary_file}"
}

summary_value_or() {
  local key=$1
  local fallback=$2
  summary_value "${key}" || printf '%s\n' "${fallback}"
}

runtime_status=$(summary_value_or runtime_status missing)
roi_present=$(summary_value_or roi_present 0)
done_present=$(summary_value_or done_present 0)
done_status=$(summary_value_or done_status missing)
done_checksum=$(summary_value_or done_checksum missing)
trap_state=$(summary_value_or trap missing)

failed=0

expect_eq() {
  local name=$1
  local actual=$2
  local expected=$3
  if [[ "${actual}" != "${expected}" ]]; then
    echo "AUDIT_FAIL ${name}: expected=${expected} actual=${actual}" >&2
    failed=1
  fi
}

expect_eq runtime_status "${runtime_status}" 0
expect_eq roi_present "${roi_present}" 1
expect_eq done_present "${done_present}" 1
expect_eq done_status "${done_status}" 0
expect_eq done_checksum "${done_checksum}" "${expected_checksum}"
expect_eq trap "${trap_state}" good

echo "audit_log=${log_path}"
cat "${summary_file}"

if [[ -n "${snapshot_manifest}" ]]; then
  echo "snapshot_manifest=${snapshot_manifest}"
  if [[ ! -f "${snapshot_manifest}" ]]; then
    echo "AUDIT_FAIL snapshot_manifest: missing" >&2
    failed=1
  else
    if ! (
      cd "${APP}"
      grep -E '^[0-9a-f]{64}[[:space:]]+' "${snapshot_manifest}" | sha256sum -c -
    ); then
      echo "AUDIT_FAIL snapshot_manifest_sha256" >&2
      failed=1
    fi

    snapshot_prefix_value=$(awk -F '=' '$1 == "snapshot_prefix" { print $2; found = 1; exit } END { if (!found) exit 1 }' "${snapshot_manifest}" || true)
    run_image_value=$(awk -F '=' '$1 == "run_image" { print $2; found = 1; exit } END { if (!found) exit 1 }' "${snapshot_manifest}" || true)
    if [[ -n "${run_image_value}" ]]; then
      echo "manifest_run_image=${run_image_value}"
      if [[ ! -f "${run_image_value}" ]]; then
        echo "AUDIT_FAIL manifest_run_image: missing_file=${run_image_value}" >&2
        failed=1
      fi
      if [[ -n "${snapshot_prefix_value}" ]]; then
        expected_run_image="${snapshot_prefix_value}.bin"
        if [[ "${run_image_value}" != "${expected_run_image}" ]]; then
          echo "AUDIT_FAIL manifest_run_image: expected=${expected_run_image} actual=${run_image_value}" >&2
          failed=1
        else
          echo "AUDIT_PASS manifest_run_image: ${run_image_value}"
        fi
      fi
    fi

    for expected_manifest_line in "${manifest_expectations[@]}"; do
      if [[ "${expected_manifest_line}" != *=* ]]; then
        echo "AUDIT_FAIL manifest_expectation: invalid=${expected_manifest_line}" >&2
        failed=1
        continue
      fi
      if ! grep -Fxq -- "${expected_manifest_line}" "${snapshot_manifest}"; then
        echo "AUDIT_FAIL manifest_expectation: missing=${expected_manifest_line}" >&2
        failed=1
      else
        echo "AUDIT_PASS manifest_expectation: ${expected_manifest_line}"
      fi
    done
  fi
elif [[ "${#manifest_expectations[@]}" -gt 0 ]]; then
  echo "AUDIT_FAIL manifest_expectation: --snapshot-manifest is required" >&2
  failed=1
fi

if [[ "${failed}" -ne 0 ]]; then
  echo "audit_status=incomplete_or_invalid"
  exit 3
fi

echo "audit_status=valid_completed_emu"

"${APP}/scripts/extract_emu_perf_counters.sh" --log "${log_path}" \
  ': commitInstr$' \
  ': commitInstrLoad$' \
  ': commitInstrStore$' \
  ': jmp_instr_cnt$' \
  ': brh_instr_cnt$' \
  ': alu_instr_cnt$' \
  ': mul_instr_cnt$' \
  ': div_instr_cnt$' \
  ': int_to_vector_instr_cnt$' \
  ': vsetiwi_instr_cnt$' \
  ': vimac_instr_cnt$'

for entry in "${baselines[@]}"; do
  if [[ "${entry}" != *=* ]]; then
    echo "Invalid baseline entry: ${entry}" >&2
    exit 2
  fi
  name=${entry%%=*}
  baseline_log=${entry#*=}
  echo "baseline_compare=${name}"
  "${APP}/scripts/compare_emu_roi.sh" "${baseline_log}" "${log_path}"
done
