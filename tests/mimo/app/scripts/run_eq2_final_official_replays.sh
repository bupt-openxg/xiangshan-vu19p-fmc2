#!/usr/bin/env bash
set -euo pipefail

APP="${MIMO_APP_DIR:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)}"
EMU_PATTERN="${ROOT}/XiangShan/build/emu"
RUN_ROOT=${APP}/perf_eval/raw/emu/final-replay
POLL_INTERVAL_SEC=${MIMO_EMU_REPLAY_POLL_INTERVAL_SEC:-300}
ALLOW_REBASELINE=${MIMO_EQ2_REPLAY_ALLOW_REBASELINE:-0}

usage() {
  cat <<'EOF'
Usage:
  scripts/run_eq2_final_official_replays.sh

The launcher copies itself into a timestamped run directory and starts a
detached, serial replay of the six accepted EQ2 profiles. Official workers use
warmup/repeat/timeout=0/1/0. Only one worker is active at a time.
EOF
}

field_value() {
  local line=$1
  local key=$2

  awk -v key="${key}" '
    {
      for (i = 1; i <= NF; ++i) {
        split($i, pair, "=")
        if (pair[1] == key) {
          sub("^[^=]*=", "", $i)
          print $i
          exit
        }
      }
    }
  ' <<< "${line}"
}

summary_value() {
  local summary=$1
  local key=$2

  awk -F '=' -v key="${key}" '$1 == key { print $2; exit }' "${summary}"
}

worker_is_running() {
  local pid=$1
  local job_id=$2
  local state=
  local args=

  kill -0 "${pid}" 2>/dev/null || return 1
  state=$(ps -p "${pid}" -o stat= 2>/dev/null | tr -d '[:space:]' || true)
  [[ -n "${state}" && "${state}" != Z* ]] || return 1
  args=$(ps -p "${pid}" -o args= 2>/dev/null || true)
  [[ "${args}" == *"${job_id}"* ]]
}

submit_profile() {
  local backend=$1
  local kernel=$2
  local submit_output=
  local submit_status=0

  case "${backend}:${kernel}" in
    scalar:eq_zf_2x2)
      submit_output=$(env \
        MIMO_USE_EQ2_ZF_DIRECT_H=1 \
        MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP=1 \
        TIMEOUT_SEC=0 \
        bash "${APP}/scripts/submit_eq2_scalar_official.sh" "${kernel}" 2>&1)
      ;;
    rvv:eq_zf_2x2)
      submit_output=$(env \
        MIMO_USE_EQ2_ZF_DIRECT_H=1 \
        MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP=1 \
        MIMO_USE_RVV_EQ2_IMPLICIT_VXRM=1 \
        TIMEOUT_SEC=0 \
        bash "${APP}/scripts/submit_eq2_rvv_official.sh" "${kernel}" wide 2>&1)
      ;;
    ext:eq_zf_2x2)
      submit_output=$(env \
        MIMO_USE_EQ2_ZF_DIRECT_H=1 \
        MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID=1 \
        TIMEOUT_SEC=0 \
        bash "${APP}/scripts/submit_eq2_ext_official.sh" "${kernel}" m2 2>&1)
      ;;
    scalar:eq_mmse_2x2)
      submit_output=$(env \
        MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA=1 \
        MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE=1 \
        TIMEOUT_SEC=0 \
        bash "${APP}/scripts/submit_eq2_scalar_official.sh" "${kernel}" 2>&1)
      ;;
    rvv:eq_mmse_2x2)
      submit_output=$(env \
        MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT=1 \
        MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD=0 \
        MIMO_USE_RVV_EQ2_IMPLICIT_VXRM=0 \
        MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP=1 \
        MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE=1 \
        TIMEOUT_SEC=0 \
        bash "${APP}/scripts/submit_eq2_rvv_official.sh" "${kernel}" wide 2>&1)
      ;;
    ext:eq_mmse_2x2)
      submit_output=$(env \
        MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13=1 \
        TIMEOUT_SEC=0 \
        bash "${APP}/scripts/submit_eq2_ext_official.sh" "${kernel}" m2 2>&1)
      ;;
    *)
      echo "REPLAY_FAIL unsupported_profile=${backend}:${kernel}" >&2
      return 2
      ;;
  esac

  submit_status=$?
  if (( submit_status != 0 )); then
    printf '%s\n' "${submit_output}" >&2
    return "${submit_status}"
  fi

  printf '%s\n' "${submit_output}"
}

declare -A FINAL_LOG
declare -A FINAL_MANIFEST

run_case() {
  local backend=$1
  local kernel=$2
  local expected_cycle=$3
  local expected_instret=$4
  local expected_checksum=$5
  local expected_store_trust=$6
  local submit_output=
  local submitted_line=
  local job_id=
  local worker_pid=
  local driver_log=
  local wall_line=
  local runtime_status=
  local raw_log=
  local manifest=
  local audit_log=
  local summary=
  local actual_cycle=
  local actual_instret=
  local actual_checksum=
  local key="${kernel}:${backend}"

  printf 'REPLAY_CASE_BEGIN time=%s backend=%s kernel=%s\n' \
    "$(date -Is)" "${backend}" "${kernel}"

  if ! submit_output=$(submit_profile "${backend}" "${kernel}"); then
    printf '%s\n' "${submit_output}" >&2
    echo "REPLAY_FAIL submit backend=${backend} kernel=${kernel}" >&2
    return 1
  fi
  printf '%s\n' "${submit_output}"

  submitted_line=$(awk '/^MIMO_EMU_JOB_SUBMITTED / { line = $0 } END { print line }' \
    <<< "${submit_output}")
  job_id=$(field_value "${submitted_line}" job_id)
  worker_pid=$(field_value "${submitted_line}" pid)
  driver_log=$(field_value "${submitted_line}" driver_log)
  if [[ -z "${job_id}" || ! "${worker_pid}" =~ ^[0-9]+$ || -z "${driver_log}" ]]; then
    echo "REPLAY_FAIL malformed_submission backend=${backend} kernel=${kernel}" >&2
    return 1
  fi

  printf 'REPLAY_WAIT job_id=%s pid=%s poll_interval_sec=%s\n' \
    "${job_id}" "${worker_pid}" "${POLL_INTERVAL_SEC}"
  while worker_is_running "${worker_pid}" "${job_id}"; do
    sleep "${POLL_INTERVAL_SEC}"
  done

  for _ in $(seq 1 30); do
    if grep -q 'MIMO_EMU_WALLTIME ' "${driver_log}" 2>/dev/null; then
      break
    fi
    sleep 2
  done

  wall_line=$(grep 'MIMO_EMU_WALLTIME ' "${driver_log}" 2>/dev/null | tail -n 1 || true)
  runtime_status=$(field_value "${wall_line}" status)
  raw_log=$(field_value "${wall_line}" log)
  manifest="${APP}/perf_eval/raw/emu/jobs/${job_id}.mimo-${backend}.manifest.txt"
  if [[ "${runtime_status}" != "0" || -z "${raw_log}" || ! -f "${raw_log}" ]]; then
    echo "REPLAY_FAIL incomplete_worker job_id=${job_id} status=${runtime_status:-missing}" >&2
    return 1
  fi

  audit_log="${REPLAY_RUN_DIR}/${backend}-${kernel}.audit.txt"
  if ! "${APP}/scripts/audit_completed_emu_result.sh" \
      --log "${raw_log}" \
      --expected-checksum "${expected_checksum}" \
      --snapshot-manifest "${manifest}" \
      --expect-manifest "backend=${backend}" \
      --expect-manifest "kernel=${kernel}" \
      --expect-manifest warmup=0 \
      --expect-manifest repeat=1 \
      --expect-manifest MIMO_VALIDATE_TOL=11 \
      --expect-manifest "MIMO_USE_SOLVE_STORE_TRUST_I16=${expected_store_trust}" \
      --expect-manifest MIMO_EMU_REQUIRE_NO_HARD_DIV=1 \
      > "${audit_log}" 2>&1; then
    cat "${audit_log}" >&2
    echo "REPLAY_FAIL completed_audit job_id=${job_id}" >&2
    return 1
  fi

  summary="${REPLAY_RUN_DIR}/${backend}-${kernel}.summary.txt"
  "${APP}/scripts/summarize_emu_results.sh" --log "${raw_log}" > "${summary}"
  actual_cycle=$(summary_value "${summary}" roi_cycle)
  actual_instret=$(summary_value "${summary}" roi_instret)
  actual_checksum=$(summary_value "${summary}" done_checksum)
  if [[ "${actual_cycle}" != "${expected_cycle}" ||
        "${actual_instret}" != "${expected_instret}" ||
        "${actual_checksum}" != "${expected_checksum}" ]]; then
    if [[ "${ALLOW_REBASELINE}" != "1" ||
          "${actual_instret}" != "${expected_instret}" ||
          "${actual_checksum}" != "${expected_checksum}" ]]; then
      printf 'REPLAY_FAIL frontier_mismatch job_id=%s expected=%s/%s/%s actual=%s/%s/%s\n' \
        "${job_id}" "${expected_cycle}" "${expected_instret}" "${expected_checksum}" \
        "${actual_cycle}" "${actual_instret}" "${actual_checksum}" >&2
      return 1
    fi
    printf 'REPLAY_CASE_REBASELINE job_id=%s expected_cycle=%s actual_cycle=%s instret=%s checksum=%s\n' \
      "${job_id}" "${expected_cycle}" "${actual_cycle}" "${actual_instret}" \
      "${actual_checksum}"
  fi

  FINAL_LOG["${key}"]=${raw_log}
  FINAL_MANIFEST["${key}"]=${manifest}
  printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' \
    "${backend}" "${kernel}" "${job_id}" "${actual_cycle}" \
    "${actual_instret}" "${actual_checksum}" "${raw_log}" "${manifest}" \
    >> "${REPLAY_RUN_DIR}/results.tsv"
  printf 'REPLAY_CASE_PASS time=%s job_id=%s cycle=%s instret=%s checksum=%s\n' \
    "$(date -Is)" "${job_id}" "${actual_cycle}" "${actual_instret}" \
    "${actual_checksum}"
}

run_fairness() {
  local kernel=$1
  local output="${REPLAY_RUN_DIR}/${kernel}.fairness.txt"
  local scalar_key="${kernel}:scalar"
  local rvv_key="${kernel}:rvv"
  local ext_key="${kernel}:ext"

  if ! "${APP}/scripts/audit_emu_backend_fairness.sh" \
      --entry "scalar=${FINAL_LOG[${scalar_key}]},${FINAL_MANIFEST[${scalar_key}]}" \
      --entry "rvv=${FINAL_LOG[${rvv_key}]},${FINAL_MANIFEST[${rvv_key}]}" \
      --entry "ext=${FINAL_LOG[${ext_key}]},${FINAL_MANIFEST[${ext_key}]}" \
      > "${output}" 2>&1; then
    cat "${output}" >&2
    echo "REPLAY_FAIL fairness kernel=${kernel}" >&2
    return 1
  fi
  printf 'REPLAY_FAIRNESS_PASS kernel=%s output=%s\n' "${kernel}" "${output}"
}

worker_main() {
  local lock_file="${RUN_ROOT}/eq2-final-official-replay.lock"

  mkdir -p "${RUN_ROOT}"
  exec 9>"${lock_file}"
  if ! flock -n 9; then
    echo "REPLAY_FAIL another_final_replay_is_active lock=${lock_file}" >&2
    exit 3
  fi
  if pgrep -f "${EMU_PATTERN}" >/dev/null; then
    echo "REPLAY_FAIL emu_already_running" >&2
    exit 3
  fi

  printf 'backend\tkernel\tjob_id\tcycle\tinstret\tchecksum\traw_log\tmanifest\n' \
    > "${REPLAY_RUN_DIR}/results.tsv"
  printf 'REPLAY_BEGIN time=%s run_dir=%s poll_interval_sec=%s allow_rebaseline=%s\n' \
    "$(date -Is)" "${REPLAY_RUN_DIR}" "${POLL_INTERVAL_SEC}" "${ALLOW_REBASELINE}"

  run_case scalar eq_zf_2x2 498205 1716048 -183047995 0
  run_case rvv eq_zf_2x2 261680 202839 -183047995 0
  run_case ext eq_zf_2x2 168680 79985 -1806027835 1
  run_fairness eq_zf_2x2

  run_case scalar eq_mmse_2x2 685098 2215253 -1091216539 0
  run_case rvv eq_mmse_2x2 558922 251582 -1091216539 0
  run_case ext eq_mmse_2x2 262365 99476 -590152251 0
  run_fairness eq_mmse_2x2

  if [[ "${ALLOW_REBASELINE}" == "1" ]]; then
    printf 'REPLAY_REBASELINE_COMPLETE time=%s results=%s\n' \
      "$(date -Is)" "${REPLAY_RUN_DIR}/results.tsv"
  else
    printf 'REPLAY_ALL_PASS time=%s results=%s\n' \
      "$(date -Is)" "${REPLAY_RUN_DIR}/results.tsv"
  fi
}

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
  usage
  exit 0
fi

if ! [[ "${POLL_INTERVAL_SEC}" =~ ^[0-9]+$ ]] || (( POLL_INTERVAL_SEC < 300 )); then
  echo "MIMO_EMU_REPLAY_POLL_INTERVAL_SEC must be an integer >= 300." >&2
  exit 2
fi
if [[ "${ALLOW_REBASELINE}" != "0" && "${ALLOW_REBASELINE}" != "1" ]]; then
  echo "MIMO_EQ2_REPLAY_ALLOW_REBASELINE must be 0 or 1." >&2
  exit 2
fi

if [[ "${1:-}" == "--worker" ]]; then
  REPLAY_RUN_DIR=${2:-}
  case "${REPLAY_RUN_DIR}" in
    "${RUN_ROOT}"/*) ;;
    *) echo "Invalid replay run directory: ${REPLAY_RUN_DIR}" >&2; exit 2 ;;
  esac
  worker_main
  exit 0
fi

if [[ $# -ne 0 ]]; then
  usage >&2
  exit 2
fi
if pgrep -f "${EMU_PATTERN}" >/dev/null; then
  echo "Refusing final replay launch: an official emu process is already running." >&2
  exit 3
fi

run_id=$(date +%Y%m%d-%H%M%S)-eq2-final-replay
run_dir="${RUN_ROOT}/${run_id}"
worker_copy="${run_dir}/run_eq2_final_official_replays.sh"
orchestrator_log="${run_dir}/orchestrator.log"
mkdir -p "${run_dir}"
cp -- "$0" "${worker_copy}"
chmod +x "${worker_copy}"

nohup setsid env \
  MIMO_EMU_REPLAY_POLL_INTERVAL_SEC="${POLL_INTERVAL_SEC}" \
  MIMO_EQ2_REPLAY_ALLOW_REBASELINE="${ALLOW_REBASELINE}" \
  bash "${worker_copy}" --worker "${run_dir}" \
  > "${orchestrator_log}" 2>&1 < /dev/null &
orchestrator_pid=$!

sleep 1
if ! kill -0 "${orchestrator_pid}" 2>/dev/null; then
  echo "Final replay orchestrator failed during launch: ${orchestrator_log}" >&2
  sed -n '1,120p' "${orchestrator_log}" >&2
  exit 1
fi

printf 'MIMO_EQ2_FINAL_REPLAY_SUBMITTED run_id=%s pid=%s poll_interval_sec=%s log=%s\n' \
  "${run_id}" "${orchestrator_pid}" "${POLL_INTERVAL_SEC}" "${orchestrator_log}"
