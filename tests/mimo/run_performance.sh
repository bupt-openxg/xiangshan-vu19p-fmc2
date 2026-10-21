#!/usr/bin/env bash
# =============================================================================
# 测试用例二 / 流程二：MIMO 均衡性能测试（4 配置 × 2 实现 = 8 次）
#   规模: 100 RB × 12 子载波 × 13 符号 = 15600 频域位置
#   指标: (RVV 单次平均周期 - RVV+EXT 单次平均周期) / RVV ≥ 30%
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
source "$ROOT/tests/common/build_env.sh"

OUT_DIR="$OUT/mimo-performance"
mkdir -p "$OUT_DIR"
CAPSEC="${MIMO_CAPSEC:-240}"
WARMUP="${MIMO_WARMUP_REPEAT:-4}"
REPEAT="${MIMO_REPEAT:-10}"
printf 'impl\tkernel\tcycle\trepeat\taverage\n' > "$OUT_DIR/raw.tsv"

fail=0
for impl in rvv ext; do
    for kernel in eq_zf_2x2 eq_mmse_2x2 eq_zf_4x4 eq_mmse_4x4; do
        case_dir="$OUT_DIR/cases/$kernel/$impl"
        mkdir -p "$case_dir"
        bin="$case_dir/mimo.bin"
        echo "[mimo-perf] $kernel $impl"
        # 官方 run_acceptance_test.sh performance 使用的基线/扩展配置
        extra=()
        case "$kernel/$impl" in
            eq_zf_2x2/rvv)
                extra=(MIMO_USE_EQ2_ZF_DIRECT_H=1
                       MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP=1
                       MIMO_USE_RVV_EQ2_IMPLICIT_VXRM=1) ;;
            eq_zf_2x2/ext)
                extra=(MIMO_USE_EQ2_ZF_DIRECT_H=1
                       MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID=1) ;;
            eq_mmse_2x2/rvv)
                extra=(MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT=1
                       MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD=0
                       MIMO_USE_RVV_EQ2_IMPLICIT_VXRM=0
                       MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP=1
                       MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE=1) ;;
            eq_mmse_2x2/ext)
                extra=(MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13=1) ;;
        esac
        if ! build_mimo_case "$impl" "$kernel" 1 "$bin" \
                MIMO_WARMUP_REPEAT="$WARMUP" MIMO_REPEAT="$REPEAT" "${extra[@]}" \
                > "$case_dir/build.log" 2>&1; then
            printf '%s\t%s\t0\t%s\t0\n' "$impl" "$kernel" "$REPEAT" >> "$OUT_DIR/raw.tsv"
            fail=$((fail + 1)); continue
        fi
        if ! "$ROOT/tests/common/board_run.sh" "$bin" "$case_dir/run.log" "$CAPSEC"; then
            printf '%s\t%s\t0\t%s\t0\n' "$impl" "$kernel" "$REPEAT" >> "$OUT_DIR/raw.tsv"
            fail=$((fail + 1)); continue
        fi
        line=$(grep -m1 "MIMO_ROI kernel=$kernel" "$case_dir/run.log" || true)
        cycle=$(sed -n 's/.*cycle=\([0-9]*\).*/\1/p' <<< "$line")
        rep=$(sed -n 's/.*repeat=\([0-9]*\).*/\1/p' <<< "$line")
        cycle=${cycle:-0}; rep=${rep:-0}
        avg=0
        [ "$rep" -gt 0 ] 2>/dev/null && avg=$((cycle / rep))
        printf '%s\t%s\t%s\t%s\t%s\n' "$impl" "$kernel" "$cycle" "$rep" "$avg" >> "$OUT_DIR/raw.tsv"
        printf 'MIMO_ROI cycle=%s repeat=%s average=%s\n' "$cycle" "$rep" "$avg" > "$case_dir/result.txt"
        [ "$avg" -gt 0 ] || fail=$((fail + 1))
    done
done

{
    echo "MIMO PERFORMANCE TEST SUMMARY (FPGA)"
    printf '%-14s %-12s %-12s %-10s %-6s\n' CONFIGURATION RVV RVV_EXT REDUCTION STATUS
    for kernel in eq_zf_2x2 eq_mmse_2x2 eq_zf_4x4 eq_mmse_4x4; do
        case "$kernel" in
            eq_zf_2x2) label=2x2-ZF ;;
            eq_mmse_2x2) label=2x2-MMSE ;;
            eq_zf_4x4) label=4x4-ZF ;;
            *) label=4x4-MMSE ;;
        esac
        rvv=$(awk -F'\t' -v k="$kernel" '$1=="rvv" && $2==k {print $5}' "$OUT_DIR/raw.tsv")
        ext=$(awk -F'\t' -v k="$kernel" '$1=="ext" && $2==k {print $5}' "$OUT_DIR/raw.tsv")
        if [ -z "$rvv" ] || [ -z "$ext" ] || [ "$rvv" -eq 0 ]; then
            printf '%-14s %-12s %-12s %-10s %-6s\n' "$label" "${rvv:-NA}" "${ext:-NA}" NA FAIL
            fail=$((fail + 1)); continue
        fi
        reduction=$(awk -v a="$rvv" -v b="$ext" 'BEGIN{printf "%.2f", (a-b)*100.0/a}')
        status=PASS
        awk -v r="$reduction" 'BEGIN{exit !(r < 30)}' && status=FAIL
        [ "$status" = FAIL ] && fail=$((fail + 1))
        printf '%-14s %-12s %-12s %-10s %-6s\n' "$label" "$rvv" "$ext" "$reduction%" "$status"
    done
    printf '\ntotal_configurations=4 threshold=30%% status=%s\n' "$([ "$fail" -eq 0 ] && echo PASS || echo FAIL)"
} > "$OUT_DIR/summary.log"
cat "$OUT_DIR/summary.log"

[ "$fail" -eq 0 ]
