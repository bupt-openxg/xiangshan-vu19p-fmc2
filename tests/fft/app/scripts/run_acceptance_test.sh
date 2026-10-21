#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
XS_ROOT="$(cd -- "${APP_DIR}/../../.." && pwd)"

usage() {
  cat >&2 <<'EOF'
Usage:
  scripts/run_acceptance_test.sh functional OUTPUT_DIR
  scripts/run_acceptance_test.sh performance OUTPUT_DIR

Environment overrides for a reduced smoke run:
  FFT_TEST_POINTS=64,128,...,4096
  FFT_TEST_IDFTS=0,1
  FFT_TEST_IMPLS=dft_rvv,dft_ext
EOF
  exit 2
}

[[ $# -eq 2 ]] || usage
mode=$1
output_dir=$(realpath -m -- "$2")
case "$mode" in functional|performance) ;; *) usage ;; esac

[[ -f "${XS_ROOT}/env.sh" ]] || { echo "missing xs-env environment: ${XS_ROOT}/env.sh" >&2; exit 3; }
[[ -x "${XS_ROOT}/NEMU/build/riscv64-nemu-interpreter" ]] || {
  echo "missing NEMU: ${XS_ROOT}/NEMU/build/riscv64-nemu-interpreter" >&2; exit 3;
}
if [[ "$mode" == performance && ! -x "${XS_ROOT}/XiangShan/build/emu" ]]; then
  echo "missing KMHv2 simulator: ${XS_ROOT}/XiangShan/build/emu" >&2
  exit 3
fi
if [[ -d "$output_dir" ]] &&
   find "$output_dir" -mindepth 1 -maxdepth 1 -print -quit | grep -q .; then
  echo "output directory must be empty: ${output_dir}" >&2
  exit 3
fi
mkdir -p "$output_dir"

points=${FFT_TEST_POINTS:-64,128,256,512,1024,2048,4096}
idfts=${FFT_TEST_IDFTS:-0,1}
impls=${FFT_TEST_IMPLS:-dft_rvv,dft_ext}
run_id="acceptance-${mode}-$(date +%Y%m%d-%H%M%S)"

source_guard="${TMPDIR:-/tmp}/xs-env-acceptance-$(printf '%s' "$XS_ROOT" | sha256sum | cut -c1-16).lock"
if ! command -v flock >/dev/null 2>&1; then
  echo "missing required command: flock" >&2
  exit 3
fi
exec 9>"$source_guard"
if ! flock -n 9; then
  echo "another FFT or MIMO acceptance test is using this source tree" >&2
  exit 3
fi

if [[ "$mode" == functional ]]; then
  FFT_PREFLIGHT_IMPLS="$impls" \
  FFT_PREFLIGHT_POINTS="$points" \
  FFT_PREFLIGHT_IDFTS="$idfts" \
    bash "${SCRIPT_DIR}/run_fft_preflight_matrix.sh" FFT-EXP-ACCEPTANCE "$run_id"
  evidence="${APP_DIR}/evidence/FFT-EXP-ACCEPTANCE/${run_id}"
  source_results="${evidence}/matrix.tsv"
  cases_dir="${output_dir}/cases"
  mkdir -p "$cases_dir"
  detail_index=$(mktemp)
  trap 'rm -f -- "$detail_index"' EXIT
  awk -F '\t' 'NR > 1 { print }' "$source_results" > "$detail_index"
  while IFS=$'\t' read -r impl point idft corpus child_root rc row_status; do
    direction=DFT
    [[ "$idft" == 1 ]] && direction=IDFT
    implementation=rvv
    [[ "$impl" == dft_ext ]] && implementation=rvv-ext
    case_dir="${cases_dir}/${point}/${direction}/${corpus}/${implementation}"
    mkdir -p "$case_dir"
    cp -- "${child_root}/logs/build.log" "${case_dir}/build.log"
    cp -- "${child_root}/logs/nemu.log" "${case_dir}/run.log"
    cp -- "${child_root}/correctness/result.json" "${case_dir}/result.json"
  done < "$detail_index"
  concise="${output_dir}/summary.log"
  summary_source="$source_results"
  total=$(awk -F '\t' 'NR > 1 { ++n } END { print n + 0 }' "$summary_source")
  passed=$(awk -F '\t' 'NR > 1 && $7 == "PASS" { ++n } END { print n + 0 }' "$summary_source")
  failed=$((total - passed))
  status=PASS
  [[ "$failed" -eq 0 ]] || status=FAIL
  {
    printf 'FUNCTIONAL TEST SUMMARY\n'
    printf '%-20s %-6s %-8s %-8s %-6s\n' CONFIGURATION INPUTS RVV RVV_EXT STATUS
    IFS=',' read -ra point_list <<< "$points"
    IFS=',' read -ra idft_list <<< "$idfts"
    for point in "${point_list[@]}"; do
      for idft in "${idft_list[@]}"; do
        direction=DFT
        [[ "$idft" == 1 ]] && direction=IDFT
        configuration="${point}-${direction}"
        rvv_total=$(awk -F '\t' -v p="$point" -v i="$idft" 'NR>1 && $1=="dft_rvv" && $2==p && $3==i {n++} END{print n+0}' "$summary_source")
        rvv_pass=$(awk -F '\t' -v p="$point" -v i="$idft" 'NR>1 && $1=="dft_rvv" && $2==p && $3==i && $7=="PASS" {n++} END{print n+0}' "$summary_source")
        ext_total=$(awk -F '\t' -v p="$point" -v i="$idft" 'NR>1 && $1=="dft_ext" && $2==p && $3==i {n++} END{print n+0}' "$summary_source")
        ext_pass=$(awk -F '\t' -v p="$point" -v i="$idft" 'NR>1 && $1=="dft_ext" && $2==p && $3==i && $7=="PASS" {n++} END{print n+0}' "$summary_source")
        row_status=PASS
        [[ "$rvv_total" -gt 0 && "$ext_total" -gt 0 && "$rvv_pass" -eq "$rvv_total" && "$ext_pass" -eq "$ext_total" ]] || row_status=FAIL
        printf '%-20s %-6s %d/%-6d %d/%-6d %-6s\n' "$configuration" "$rvv_total" "$rvv_pass" "$rvv_total" "$ext_pass" "$ext_total" "$row_status"
      done
    done
    printf '\ntotal_cases=%s pass=%s fail=%s status=%s\n' "$total" "$passed" "$failed" "$status"
  } > "$concise"
  [[ "$status" == PASS ]] || { echo "FFT functional verification failed: ${failed} cases" >&2; exit 1; }
else
  FFT_OFFICIAL_IMPLS="$impls" \
  FFT_OFFICIAL_POINTS="$points" \
  FFT_OFFICIAL_IDFTS="$idfts" \
    bash "${SCRIPT_DIR}/run_fft_official_matrix.sh" FFT-EXP-ACCEPTANCE "$run_id"
  evidence="${APP_DIR}/evidence/FFT-EXP-ACCEPTANCE/${run_id}"
  source_results="${evidence}/official/matrix.tsv"
  cases_dir="${output_dir}/cases"
  mkdir -p "$cases_dir"
  while IFS=$'\t' read -r impl point idft corpus child_root rc row_status cycle instret; do
    [[ "$impl" == impl ]] && continue
    direction=DFT
    [[ "$idft" == 1 ]] && direction=IDFT
    implementation=rvv
    [[ "$impl" == dft_ext ]] && implementation=rvv-ext
    case_dir="${cases_dir}/${point}/${direction}/${implementation}"
    mkdir -p "$case_dir"
    cp -- "${child_root}/logs/build.log" "${case_dir}/build.log"
    cp -- "${child_root}/official/emu.report.txt" "${case_dir}/run.log"
    cp -- "${child_root}/official/result.json" "${case_dir}/result.json"
  done < "$source_results"
  raw_summary=$(mktemp)
  trap 'rm -f -- "$raw_summary"' EXIT
  set +e
  IFS=',' read -ra point_list <<< "$points"
  IFS=',' read -ra idft_list <<< "$idfts"
  expected_configurations=$(( ${#point_list[@]} * ${#idft_list[@]} ))
  python3 "${SCRIPT_DIR}/summarize_acceptance_performance.py" \
    --matrix "$source_results" \
    --output "$raw_summary" \
    --threshold 30 \
    --expected-configurations "$expected_configurations"
  summary_status=$?
  set -e
  [[ -s "$raw_summary" ]] || {
    echo "FFT performance summary was not generated" >&2
    exit 1
  }
  concise="${output_dir}/summary.log"
  awk -F '\t' '
    NR==1 {
      print "PERFORMANCE TEST SUMMARY"
      printf "%-20s %-10s %-10s %-9s %-6s\n", "CONFIGURATION", "RVV", "RVV_EXT", "REDUCTION", "STATUS"
      next
    }
    {
      printf "%-20s %-10s %-10s %8s%% %-6s\n", $1 "-" $2, $3, $4, $5, $6
      total++; if ($6=="PASS") passed++
    }
    END {
      failed=total-passed; status=(failed==0 ? "PASS" : "FAIL")
      printf "\nconfigurations=%d pass=%d fail=%d status=%s\n", total, passed, failed, status
    }
  ' "$raw_summary" > "$concise"
  if [[ "$summary_status" -ne 0 ]]; then
    echo "FFT performance threshold failed; see ${concise}" >&2
    exit "$summary_status"
  fi
fi

echo "FFT_ACCEPTANCE_TEST_PASS mode=${mode} summary=${concise} cases=${cases_dir}"
