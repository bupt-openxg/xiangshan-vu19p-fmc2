#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
ROOT="$(cd -- "${APP}/../../.." && pwd)"

usage() {
  cat >&2 <<'EOF'
Usage:
  scripts/run_acceptance_test.sh functional OUTPUT_DIR
  scripts/run_acceptance_test.sh performance OUTPUT_DIR

Set MIMO_ACCEPTANCE_RESUME=1 to resume an interrupted performance run from
validated case results already present in OUTPUT_DIR.

The functional flow validates RVV and RVV-with-extension implementations of
2x2/4x4 ZF and MMSE kernels. The performance flow runs the accepted whole-core
profiles and writes cycle counts plus RVV-to-extension reductions to OUTPUT_DIR.
EOF
  exit 2
}

[[ $# -eq 2 ]] || usage
mode=$1
output_dir=$(realpath -m -- "$2")
case "$mode" in functional|performance) ;; *) usage ;; esac
resume=${MIMO_ACCEPTANCE_RESUME:-0}
case "$resume" in 0|1) ;; *) echo "MIMO_ACCEPTANCE_RESUME must be 0 or 1" >&2; exit 2 ;; esac
if [[ "$resume" == 1 && "$mode" != performance ]]; then
  echo "MIMO_ACCEPTANCE_RESUME is supported only for performance mode" >&2
  exit 2
fi

[[ -f "${ROOT}/env.sh" ]] || { echo "missing xs-env environment: ${ROOT}/env.sh" >&2; exit 3; }
[[ -x "${ROOT}/NEMU/build/riscv64-nemu-interpreter" ]] || { echo "missing NEMU" >&2; exit 3; }
if [[ "$mode" == performance && ! -x "${ROOT}/XiangShan/build/emu" ]]; then
  echo "missing KMHv2 simulator: ${ROOT}/XiangShan/build/emu" >&2
  exit 3
fi
if [[ -d "$output_dir" ]] &&
   find "$output_dir" -mindepth 1 -maxdepth 1 -print -quit | grep -q . &&
   [[ "$resume" != 1 ]]; then
  echo "output directory must be empty: ${output_dir}" >&2
  exit 3
fi
mkdir -p "$output_dir"

export XS_ENV_ROOT="$ROOT"
export MIMO_APP_DIR="$APP"
cd "$APP"

source_guard="${TMPDIR:-/tmp}/xs-env-acceptance-$(printf '%s' "$ROOT" | sha256sum | cut -c1-16).lock"
if ! command -v flock >/dev/null 2>&1; then
  echo "missing required command: flock" >&2
  exit 3
fi
exec 9>"$source_guard"
if ! flock -n 9; then
  echo "another FFT or MIMO acceptance test is using this source tree" >&2
  exit 3
fi

run_logged() {
  local name=$1
  shift
  echo "MIMO_ACCEPTANCE_CASE_BEGIN name=${name}"
  "$@" 2>&1 | tee "${case_tmp}/${name}.log"
  echo "MIMO_ACCEPTANCE_CASE_PASS name=${name}"
}

if [[ "$mode" == functional ]]; then
  case_tmp=$(mktemp -d)
  trap 'rm -rf -- "$case_tmp"' EXIT
  functional_set=${MIMO_ACCEPTANCE_FUNCTIONAL_SET:-full}
  run_logged eq2-quality env MIMO_EQ2_QUALITY_TOL=11 TIMEOUT_SEC=900 \
      bash "$SCRIPT_DIR/run_eq2_quality_sweep.sh" acceptance
  grep -q 'MIMO_EQ2_QUALITY_SWEEP_DONE' "${case_tmp}/eq2-quality.log"
  if [[ "$functional_set" == full ]]; then
    run_logged zf4-rvv env MIMO_VALIDATE_TOL=11 TIMEOUT_SEC=900 MIMO_GATE_SKIP_ASM=1 \
      bash "$SCRIPT_DIR/run_zf4_rvv_current_best_gate.sh" acceptance-zf4-rvv
    run_logged zf4-ext env MIMO_VALIDATE_TOL=11 TIMEOUT_SEC=900 MIMO_GATE_SKIP_ASM=1 \
      bash "$SCRIPT_DIR/run_zf4_ext_packed_current_best_gate.sh" acceptance-zf4-ext
    run_logged mmse4-rvv env MIMO_VALIDATE_TOL=11 TIMEOUT_SEC=900 MIMO_GATE_SKIP_ASM=1 \
      bash "$SCRIPT_DIR/run_mmse4_rvv_current_best_gate.sh" acceptance-mmse4-rvv
    run_logged mmse4-ext env MIMO_VALIDATE_TOL=11 TIMEOUT_SEC=900 MIMO_GATE_SKIP_ASM=1 \
      bash "$SCRIPT_DIR/run_mmse4_ext_current_best_gate.sh" acceptance-mmse4-ext
    grep -q 'MIMO_ZF4_RVV_ALIGNED_GATE_DONE' "${case_tmp}/zf4-rvv.log"
    grep -q 'MIMO_ZF4_EXT_PACKED_HOT_GATE_PASS' "${case_tmp}/zf4-ext.log"
    grep -q 'MIMO_MMSE4_RVV_CURRENT_BEST_GATE_DONE' "${case_tmp}/mmse4-rvv.log"
    grep -q 'MIMO_MMSE4_EXT_CURRENT_BEST_GATE_DONE' "${case_tmp}/mmse4-ext.log"
  elif [[ "$functional_set" != eq2 ]]; then
    echo "unsupported MIMO functional set: $functional_set" >&2
    exit 2
  fi
  detail=$(mktemp)
  printf 'configuration\tinput_set\timplementation\tstatus\n' > "$detail"
  awk '
    BEGIN { OFS="\t" }
    $1 == "MIMO_EQ2_QUALITY" {
      delete value
      for (field=2; field<=NF; ++field) {
        split($field, pair, "=")
        value[pair[1]]=pair[2]
      }
      if (value["backend"] != "rvv" && value["backend"] != "ext") next
      configuration=(value["kernel"] == "eq_zf_2x2" ? "2x2-ZF" : "2x2-MMSE")
      input_set=(value["mod_bits"] == 2 ? "QPSK" : (value["mod_bits"] == 4 ? "16-QAM" : "64-QAM"))
      implementation=(value["backend"] == "rvv" ? "RVV" : "RVV_EXT")
      key=configuration SUBSEP input_set SUBSEP implementation
      if (!seen[key]++) print configuration, input_set, implementation, "PASS"
    }
  ' "${case_tmp}/eq2-quality.log" >> "$detail"

  append_4x4_result() {
    local configuration=$1
    local implementation=$2
    local log_file=$3
    local marker=$4
    grep -q "$marker" "$log_file"
    printf '%s\t-\t%s\tPASS\n' "$configuration" "$implementation" >> "$detail"
  }
  append_4x4_result 4x4-ZF RVV "${case_tmp}/zf4-rvv.log" \
    'MIMO_ZF4_RVV_ALIGNED_GATE_DONE'
  append_4x4_result 4x4-ZF RVV_EXT "${case_tmp}/zf4-ext.log" \
    'MIMO_ZF4_EXT_PACKED_HOT_GATE_PASS'
  append_4x4_result 4x4-MMSE RVV "${case_tmp}/mmse4-rvv.log" \
    'MIMO_MMSE4_RVV_CURRENT_BEST_GATE_DONE'
  append_4x4_result 4x4-MMSE RVV_EXT "${case_tmp}/mmse4-ext.log" \
    'MIMO_MMSE4_EXT_CURRENT_BEST_GATE_DONE'
  cases_dir="${output_dir}/cases"
  mkdir -p "$cases_dir"
  for case_args in \
    'eq_zf_2x2 ZF 2 1024 0 QPSK' \
    'eq_zf_2x2 ZF 4 2048 1 16-QAM' \
    'eq_zf_2x2 ZF 6 4096 2 64-QAM' \
    'eq_mmse_2x2 MMSE 2 1024 0 QPSK' \
    'eq_mmse_2x2 MMSE 4 2048 1 16-QAM' \
    'eq_mmse_2x2 MMSE 6 4096 2 64-QAM'; do
    read -r kernel algorithm mod_bits noise_q15 cond_profile modulation <<< "$case_args"
    for backend in rvv ext; do
      implementation=$backend
      [[ "$backend" == ext ]] && implementation=rvv-ext
      case_dir="${cases_dir}/2x2/${algorithm}/${modulation}/${implementation}"
      mkdir -p "$case_dir"
      case_tag="acceptance-${kernel}-${backend}"
      source_log="${APP}/perf_eval/raw/nemu/eq2-quality/${case_tag}-${backend}-${kernel}-m${mod_bits}-n${noise_q15}-c${cond_profile}.log"
      [[ -f "$source_log" ]] || { echo "missing MIMO functional evidence: $source_log" >&2; exit 1; }
      cp -- "$source_log" "${case_dir}/run.log"
      grep -E "MIMO_VALIDATE_(PASS|ERROR_STATS) kernel=${kernel}|MIMO_EQ2_QUALITY backend=${backend} kernel=${kernel}" \
        "$source_log" > "${case_dir}/result.log"
    done
  done
  if [[ "$functional_set" == full ]]; then
    for case_args in \
    'ZF rvv zf4-rvv MIMO_ZF4_RVV_ALIGNED_GATE_DONE' \
    'ZF rvv-ext zf4-ext MIMO_ZF4_EXT_PACKED_HOT_GATE_PASS' \
    'MMSE rvv mmse4-rvv MIMO_MMSE4_RVV_CURRENT_BEST_GATE_DONE' \
    'MMSE rvv-ext mmse4-ext MIMO_MMSE4_EXT_CURRENT_BEST_GATE_DONE'; do
    read -r algorithm implementation case_name marker <<< "$case_args"
    case_dir="${cases_dir}/4x4/${algorithm}/${implementation}"
    mkdir -p "$case_dir"
    cp -- "${case_tmp}/${case_name}.log" "${case_dir}/run.log"
    grep -E "MIMO_VALIDATE_(PASS|ERROR_STATS)|${marker}" "${case_tmp}/${case_name}.log" \
      > "${case_dir}/result.log"
    done
  fi
  concise="${output_dir}/summary.log"
  total=$(awk -F '\t' 'NR > 1 { ++n } END { print n + 0 }' "$detail")
  passed=$(awk -F '\t' 'NR > 1 && $4 == "PASS" { ++n } END { print n + 0 }' "$detail")
  failed=$((total - passed))
  status=PASS
  [[ "$failed" -eq 0 ]] || status=FAIL
  {
    printf 'FUNCTIONAL TEST SUMMARY\n'
    printf '%-20s %-6s %-8s %-8s %-6s\n' CONFIGURATION INPUTS RVV RVV_EXT STATUS
    items=( \
      '2x2 ZF QPSK 2x2-ZF-QPSK' \
      '2x2 ZF 16-QAM 2x2-ZF-16-QAM' \
      '2x2 ZF 64-QAM 2x2-ZF-64-QAM' \
      '2x2 MMSE QPSK 2x2-MMSE-QPSK' \
      '2x2 MMSE 16-QAM 2x2-MMSE-16-QAM' \
      '2x2 MMSE 64-QAM 2x2-MMSE-64-QAM' \
    )
    if [[ "$functional_set" == full ]]; then
      items+=( '4x4 ZF - 4x4-ZF' '4x4 MMSE - 4x4-MMSE' )
    fi
    for item in "${items[@]}"; do
      read -r mimo alg mod key <<< "$item"
      config="${mimo}-${alg}"
      rvv=$(awk -F '\t' -v c="$config" -v m="$mod" '$1==c && $2==m && $3=="RVV" {print $4}' "$detail")
      ext=$(awk -F '\t' -v c="$config" -v m="$mod" '$1==c && $2==m && $3=="RVV_EXT" {print $4}' "$detail")
      row_status=PASS
      [[ "$rvv" == PASS && "$ext" == PASS ]] || row_status=FAIL
      display_config="$config"
      [[ "$mod" == - ]] || display_config="${config}-${mod}"
      rvv_count=0/1
      ext_count=0/1
      [[ "$rvv" == PASS ]] && rvv_count=1/1
      [[ "$ext" == PASS ]] && ext_count=1/1
      printf '%-20s %-6s %-8s %-8s %-6s\n' "$display_config" 1 "$rvv_count" "$ext_count" "$row_status"
    done
    printf '\ntotal_cases=%s pass=%s fail=%s status=%s\n' "$total" "$passed" "$failed" "$status"
  } > "$concise"
  expected_total=12
  [[ "$functional_set" == full ]] && expected_total=16
  [[ "$total" -eq "$expected_total" && "$status" == PASS ]] || {
    echo "MIMO functional verification failed: total=${total} failed=${failed}" >&2
    exit 1
  }
  echo "MIMO_ACCEPTANCE_TEST_PASS mode=functional summary=${concise} cases=${cases_dir}"
  exit 0
fi

field_value() {
  local line=$1
  local key=$2
  awk -v key="$key" '{ for (i=1;i<=NF;++i) { split($i,a,"="); if (a[1]==key) { sub("^[^=]*=", "", $i); print $i; exit } } }' <<< "$line"
}

summary_value() {
  local file=$1
  local key=$2
  awk -F= -v key="$key" '$1==key { print $2; exit }' "$file"
}

worker_is_running() {
  local pid=$1
  local job_id=$2
  local state args
  kill -0 "$pid" 2>/dev/null || return 1
  state=$(ps -p "$pid" -o stat= 2>/dev/null | tr -d '[:space:]' || true)
  [[ -n "$state" && "$state" != Z* ]] || return 1
  args=$(ps -p "$pid" -o args= 2>/dev/null || true)
  [[ "$args" == *"$job_id"* ]]
}

declare -A CYCLE
cases_dir="${output_dir}/cases"
mkdir -p "$cases_dir"
case_tmp=$(mktemp -d)
trap 'rm -rf -- "$case_tmp"' EXIT

run_submitted_case() {
  local backend=$1
  local kernel=$2
  shift 2
  local submit_output submit_status line job_id pid driver_log wall_line status raw_log
  local summary cycle repeat average_cycle done_status trap_state implementation case_dir

  implementation=RVV
  [[ "$backend" == ext ]] && implementation=RVV_EXT
  case "$kernel" in
    eq_zf_2x2)
      configuration=2x2-ZF
      case_dir="${output_dir}/cases/2x2/ZF/${implementation}"
      ;;
    eq_mmse_2x2)
      configuration=2x2-MMSE
      case_dir="${output_dir}/cases/2x2/MMSE/${implementation}"
      ;;
    eq_zf_4x4)
      configuration=4x4-ZF
      case_dir="${output_dir}/cases/4x4/ZF/${implementation}"
      ;;
    eq_mmse_4x4)
      configuration=4x4-MMSE
      case_dir="${output_dir}/cases/4x4/MMSE/${implementation}"
      ;;
    *) echo "unsupported MIMO performance kernel: ${kernel}" >&2; return 1 ;;
  esac
  case_dir=${case_dir//RVV_EXT/rvv-ext}
  case_dir=${case_dir//RVV/rvv}

  if [[ "$resume" == 1 && ( -e "${case_dir}/run.log" || -e "${case_dir}/result.log" ) ]]; then
    [[ -f "${case_dir}/run.log" && -f "${case_dir}/result.log" ]] || {
      echo "incomplete saved MIMO case: backend=${backend} kernel=${kernel} case_dir=${case_dir}" >&2
      return 1
    }
    summary="${case_dir}/result.log"
    raw_log="${case_dir}/run.log"
    cycle=$(summary_value "$summary" roi_cycle)
    repeat=$(summary_value "$summary" roi_repeat)
    done_status=$(summary_value "$summary" done_status)
    trap_state=$(summary_value "$summary" trap)
    saved_backend=$(summary_value "$summary" runtime_backend)
    saved_kernel=$(summary_value "$summary" runtime_kernel)
    [[ "$cycle" =~ ^[0-9]+$ && "$repeat" =~ ^[0-9]+$ && "$repeat" -gt 0 &&
       "$done_status" == 0 && "$trap_state" == good &&
       "$saved_backend" == "$backend" && "$saved_kernel" == "$kernel" ]] || {
      echo "invalid saved MIMO case: backend=${backend} kernel=${kernel} case_dir=${case_dir}" >&2
      return 1
    }
    average_cycle=$(awk -v c="$cycle" -v r="$repeat" 'BEGIN { printf "%.3f", c/r }')
    average_cycle=$(sed -E 's/0+$//; s/\.$//' <<< "$average_cycle")
    CYCLE["${backend}:${kernel}"]=$average_cycle
    printf '%s\t%s\t%s\tPASS\t%s\n' "$configuration" "$implementation" "$average_cycle" "$raw_log" \
      >> "${case_tmp}/performance-results.tsv"
    echo "MIMO_ACCEPTANCE_CASE_RESUMED backend=${backend} kernel=${kernel} case_dir=${case_dir}"
    return 0
  fi

  set +e
  submit_output=$("$@" 2>&1)
  submit_status=$?
  set -e
  printf '%s\n' "$submit_output"
  [[ "$submit_status" -eq 0 ]] || {
    echo "MIMO submission failed: backend=${backend} kernel=${kernel} status=${submit_status}" >&2
    return "$submit_status"
  }
  line=$(awk '/^MIMO_EMU_JOB_SUBMITTED / { out=$0 } END { print out }' <<< "$submit_output")
  job_id=$(field_value "$line" job_id)
  pid=$(field_value "$line" pid)
  driver_log=$(field_value "$line" driver_log)
  [[ -n "$job_id" && "$pid" =~ ^[0-9]+$ && -n "$driver_log" ]] || {
    echo "malformed MIMO submission: backend=${backend} kernel=${kernel}" >&2; return 1;
  }

  while worker_is_running "$pid" "$job_id"; do sleep 30; done
  for _ in $(seq 1 30); do
    grep -q 'MIMO_EMU_WALLTIME ' "$driver_log" 2>/dev/null && break
    sleep 2
  done
  wall_line=$(grep 'MIMO_EMU_WALLTIME ' "$driver_log" 2>/dev/null | tail -n 1 || true)
  status=$(field_value "$wall_line" status)
  raw_log=$(field_value "$wall_line" log)
  [[ "$status" == 0 && -f "$raw_log" ]] || {
    echo "incomplete MIMO run: job_id=${job_id} status=${status:-missing}" >&2; return 1;
  }

  summary="${case_tmp}/${backend}-${kernel}-summary.txt"
  bash "$SCRIPT_DIR/summarize_emu_results.sh" --log "$raw_log" > "$summary"
  cycle=$(summary_value "$summary" roi_cycle)
  repeat=$(summary_value "$summary" roi_repeat)
  done_status=$(summary_value "$summary" done_status)
  trap_state=$(summary_value "$summary" trap)
  [[ "$cycle" =~ ^[0-9]+$ && "$repeat" =~ ^[0-9]+$ && "$repeat" -gt 0 && "$done_status" == 0 && "$trap_state" == good ]] || {
    echo "invalid MIMO result: backend=${backend} kernel=${kernel}" >&2; return 1;
  }
  average_cycle=$(awk -v c="$cycle" -v r="$repeat" 'BEGIN { printf "%.3f", c/r }')
  average_cycle=$(sed -E 's/0+$//; s/\.$//' <<< "$average_cycle")
  CYCLE["${backend}:${kernel}"]=$average_cycle
  printf '%s\t%s\t%s\tPASS\t%s\n' "$configuration" "$implementation" "$average_cycle" "$raw_log" \
    >> "${case_tmp}/performance-results.tsv"
  mkdir -p "$case_dir"
  cp -- "$raw_log" "${case_dir}/run.log"
  cp -- "$summary" "${case_dir}/result.log"
}

printf 'configuration\timplementation\tcycle\tstatus\tevidence\n' > "${case_tmp}/performance-results.tsv"

performance_kernels=${MIMO_ACCEPTANCE_PERFORMANCE_KERNELS:-eq_zf_2x2,eq_mmse_2x2,eq_zf_4x4,eq_mmse_4x4}
IFS=',' read -ra performance_kernel_list <<< "$performance_kernels"
for kernel in "${performance_kernel_list[@]}"; do
  case "$kernel" in
    eq_zf_2x2)
      run_submitted_case rvv "$kernel" env \
        MIMO_USE_EQ2_ZF_DIRECT_H=1 MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP=1 \
        MIMO_USE_RVV_EQ2_IMPLICIT_VXRM=1 TIMEOUT_SEC=0 \
        bash "$SCRIPT_DIR/submit_eq2_rvv_official.sh" "$kernel" wide
      run_submitted_case ext "$kernel" env \
        MIMO_USE_EQ2_ZF_DIRECT_H=1 MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID=1 TIMEOUT_SEC=0 \
        bash "$SCRIPT_DIR/submit_eq2_ext_official.sh" "$kernel" m2
      ;;
    eq_mmse_2x2)
      run_submitted_case rvv "$kernel" env \
        MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT=1 MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD=0 \
        MIMO_USE_RVV_EQ2_IMPLICIT_VXRM=0 MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP=1 \
        MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE=1 TIMEOUT_SEC=0 \
        bash "$SCRIPT_DIR/submit_eq2_rvv_official.sh" "$kernel" wide
      run_submitted_case ext "$kernel" env \
        MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13=1 TIMEOUT_SEC=0 \
        bash "$SCRIPT_DIR/submit_eq2_ext_official.sh" "$kernel" m2
      ;;
    eq_zf_4x4)
      run_submitted_case rvv "$kernel" env TIMEOUT_SEC=0 \
        bash "$SCRIPT_DIR/submit_zf4_rvv_aligned_official.sh"
      run_submitted_case ext "$kernel" env TIMEOUT_SEC=0 \
        bash "$SCRIPT_DIR/submit_zf4_ext_packed_tol11_official.sh"
      ;;
    eq_mmse_4x4)
      run_submitted_case rvv "$kernel" env TIMEOUT_SEC=0 \
        bash "$SCRIPT_DIR/submit_mmse4_rvv_current_best_official.sh"
      run_submitted_case ext "$kernel" env TIMEOUT_SEC=0 \
        bash "$SCRIPT_DIR/submit_mmse4_ext_packed_rx4_m4_tol11_official.sh"
      ;;
    *) echo "unsupported MIMO performance kernel: $kernel" >&2; exit 2 ;;
  esac
done

reductions="${case_tmp}/performance-summary.tsv"
printf 'configuration\trvv_cycle\trvv_ext_cycle\treduction_percent\tstatus\n' > "$reductions"
failed=0
for kernel in "${performance_kernel_list[@]}"; do
  rvv=${CYCLE["rvv:${kernel}"]}
  ext=${CYCLE["ext:${kernel}"]}
  reduction=$(awk -v r="$rvv" -v e="$ext" 'BEGIN { printf "%.2f", (r-e)*100.0/r }')
  status=$(awk -v x="$reduction" 'BEGIN { print (x>=30.0) ? "PASS" : "FAIL" }')
  [[ "$status" == PASS ]] || failed=$((failed+1))
  case "$kernel" in
    eq_zf_2x2) configuration=2x2-ZF ;;
    eq_mmse_2x2) configuration=2x2-MMSE ;;
    eq_zf_4x4) configuration=4x4-ZF ;;
    eq_mmse_4x4) configuration=4x4-MMSE ;;
  esac
  printf '%s\t%s\t%s\t%s\t%s\n' "$configuration" "$rvv" "$ext" "$reduction" "$status" >> "$reductions"
done
concise="${output_dir}/summary.log"
awk -F '\t' '
  NR==1 {
    print "PERFORMANCE TEST SUMMARY"
    printf "%-20s %-10s %-10s %-9s %-6s\n", "CONFIGURATION", "RVV", "RVV_EXT", "REDUCTION", "STATUS"
    next
  }
  {
    printf "%-20s %-10s %-10s %8s%% %-6s\n", $1, $2, $3, $4, $5
    total++; if ($5=="PASS") passed++
  }
  END {
    failed=total-passed; status=(failed==0 ? "PASS" : "FAIL")
    printf "\nconfigurations=%d pass=%d fail=%d status=%s\n", total, passed, failed, status
  }
' "$reductions" > "$concise"
[[ "$failed" -eq 0 ]] || {
  echo "MIMO performance threshold failed for ${failed} kernels; see ${concise}" >&2
  exit 1
}
echo "MIMO_ACCEPTANCE_TEST_PASS mode=performance summary=${concise} cases=${cases_dir}"
