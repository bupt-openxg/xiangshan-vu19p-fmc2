#!/usr/bin/env bash
# =============================================================================
# 测试用例二 / 流程一：MIMO 均衡功能验证（16 项）
#   2x2: ZF/MMSE × QPSK/16-QAM/64-QAM × RVV/EXT = 12
#   4x4: ZF/MMSE × RVV/EXT = 4
#   判定: MIMO_VALIDATE_ERROR_STATS fail=0 且 max_abs<=11；MIMO_DONE status=0
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
source "$ROOT/tests/common/build_env.sh"

OUT_DIR="$OUT/mimo-functional"
mkdir -p "$OUT_DIR"
CAPSEC="${MIMO_CAPSEC:-180}"
printf 'mimo\talgorithm\tmodulation\timpl\tstatus\tmax_abs\tfail\n' > "$OUT_DIR/cases.tsv"

run_case() { # kernel algo impl label mod_bits noise cond
    local kernel=$1 algo=$2 impl=$3 label=$4 mod=${5:-0} noise=${6:-0} cond=${7:-0}
    [ -n "$label" ] || label="-"
    local tag="$kernel-$impl${label:+-$label}"
    local case_dir="$OUT_DIR/cases/$algo/$label/$impl"
    mkdir -p "$case_dir"
    local bin="$case_dir/mimo.bin"
    local extra=()
    if [[ "$kernel" == *_2x2 ]]; then
        extra=(
            MIMO_EQ2_QUALITY_EVAL=1
            MIMO_EQ2_QUALITY_MOD_BITS="$mod"
            MIMO_EQ2_QUALITY_NOISE_Q15="$noise"
            MIMO_EQ2_QUALITY_COND_PROFILE="$cond"
        )
    fi
    echo "[mimo-func] $tag"
    if ! build_mimo_case "$impl" "$kernel" 0 "$bin" \
            MIMO_VALIDATE_TOL=11 MIMO_VALIDATE_ERROR_STATS=1 "${extra[@]}" \
            > "$case_dir/build.log" 2>&1; then
        printf '%s\t%s\t%s\t%s\tBUILD_FAIL\t-\t-\n' "$kernel" "$algo" "$label" "$impl" >> "$OUT_DIR/cases.tsv"
        return 1
    fi
    if ! "$ROOT/tests/common/board_run.sh" "$bin" "$case_dir/run.log" "$CAPSEC"; then
        printf '%s\t%s\t%s\t%s\tRUN_FAIL\t-\t-\n' "$kernel" "$algo" "$label" "$impl" >> "$OUT_DIR/cases.tsv"
        return 1
    fi
    local stats max_abs fails status
    stats=$(grep -m1 "MIMO_VALIDATE_ERROR_STATS kernel=$kernel" "$case_dir/run.log" || true)
    max_abs=$(sed -n 's/.*max_abs=\([0-9]*\).*/\1/p' <<< "$stats"); max_abs=${max_abs:-999}
    fails=$(sed -n 's/.*fail=\([0-9]*\).*/\1/p' <<< "$stats"); fails=${fails:-999}
    status=PASS
    if [ "$fails" != 0 ] || [ "$max_abs" -gt 11 ] || \
       ! grep -q "MIMO_VALIDATE_PASS kernel=$kernel" "$case_dir/run.log" || \
       ! grep -q "MIMO_DONE status=0" "$case_dir/run.log"; then
        status=FAIL
    fi
    if [[ "$kernel" == *_2x2 ]]; then
        grep -m1 "MIMO_EQ2_QUALITY" "$case_dir/run.log" > "$case_dir/quality.txt" || true
    fi
    printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\n' "$kernel" "$algo" "$label" "$impl" "$status" "$max_abs" "$fails" >> "$OUT_DIR/cases.tsv"
    [ "$status" = PASS ]
}

fail=0
for impl in rvv ext; do
    for kernel in eq_zf_2x2 eq_mmse_2x2; do
        algo=$([ "$kernel" = eq_zf_2x2 ] && echo ZF || echo MMSE)
        run_case "$kernel" "$algo" "$impl" QPSK 2 1024 0 || fail=$((fail + 1))
        run_case "$kernel" "$algo" "$impl" 16-QAM 4 2048 1 || fail=$((fail + 1))
        run_case "$kernel" "$algo" "$impl" 64-QAM 6 4096 2 || fail=$((fail + 1))
    done
    run_case eq_zf_4x4 ZF "$impl" "" || fail=$((fail + 1))
    run_case eq_mmse_4x4 MMSE "$impl" "" || fail=$((fail + 1))
done

{
    echo "MIMO FUNCTIONAL TEST SUMMARY (FPGA)"
    printf '%-18s %-10s %-10s %-6s\n' CONFIGURATION RVV RVV_EXT STATUS
    for item in \
        '2x2-ZF-QPSK eq_zf_2x2 QPSK' '2x2-ZF-16-QAM eq_zf_2x2 16-QAM' '2x2-ZF-64-QAM eq_zf_2x2 64-QAM' \
        '2x2-MMSE-QPSK eq_mmse_2x2 QPSK' '2x2-MMSE-16-QAM eq_mmse_2x2 16-QAM' '2x2-MMSE-64-QAM eq_mmse_2x2 64-QAM' \
        '4x4-ZF eq_zf_4x4 -' '4x4-MMSE eq_mmse_4x4 -'; do
        read -r label kernel mod <<< "$item"
        rvv=$(awk -F'\t' -v k="$kernel" -v m="$mod" '$1==k && $3==m && $4=="rvv" {print $5}' "$OUT_DIR/cases.tsv")
        ext=$(awk -F'\t' -v k="$kernel" -v m="$mod" '$1==k && $3==m && $4=="ext" {print $5}' "$OUT_DIR/cases.tsv")
        row=PASS; [ "$rvv" = PASS ] && [ "$ext" = PASS ] || row=FAIL
        printf '%-18s %-10s %-10s %-6s\n' "$label" "${rvv:-NA}" "${ext:-NA}" "$row"
    done
    total=$(awk -F'\t' 'NR>1 {n++} END{print n+0}' "$OUT_DIR/cases.tsv")
    passed=$(awk -F'\t' 'NR>1 && $5=="PASS" {n++} END{print n+0}' "$OUT_DIR/cases.tsv")
    printf '\ntotal_cases=%s pass=%s fail=%s status=%s\n' "$total" "$passed" "$((total - passed))" "$([ "$fail" -eq 0 ] && echo PASS || echo FAIL)"
} > "$OUT_DIR/summary.log"
cat "$OUT_DIR/summary.log"

[ "$fail" -eq 0 ]
