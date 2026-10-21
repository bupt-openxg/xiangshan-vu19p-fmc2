#!/usr/bin/env bash
# =============================================================================
# 测试用例二 / 流程二：FFT 性能测试（7 点 × 2 方向 × 2 实现 = 28 次）
#   指标: (RVV 单次平均周期 - RVV+EXT 单次平均周期) / RVV ≥ 30%
#   周期来源: 程序内 rdcycle 统计区间（与官方 HPM 口径一致：仅变换计算）
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
source "$ROOT/tests/common/build_env.sh"

OUT_DIR="$OUT/fft-performance"
mkdir -p "$OUT_DIR"
POINTS="${FFT_TEST_POINTS:-64,128,256,512,1024,2048,4096}"
IDFTS="${FFT_TEST_IDFTS:-0,1}"
CORPUS="${FFT_TEST_CORPUS:-C0-legacy-pattern-v1}"
CAPSEC="${FFT_CAPSEC:-120}"

printf 'impl\tpoints\tidft\tcycle\trepeat\taverage\n' > "$OUT_DIR/raw.tsv"

IFS=',' read -r -a point_list <<< "$POINTS"
IFS=',' read -r -a idft_list <<< "$IDFTS"

repeat_for() {
    case "$1" in
        64|128|256|512) echo 10 ;;
        1024) echo 5 ;;
        *) echo 3 ;;
    esac
}
warmup_for() {
    case "$1" in
        64|128) echo 8 ;;
        256|512|1024) echo 6 ;;
        *) echo 4 ;;
    esac
}

fail=0
for impl in dft_rvv dft_ext; do
    short=${impl#dft_}
    for points in "${point_list[@]}"; do
        repeat=$(repeat_for "$points")
        warmup=$(warmup_for "$points")
        for idft in "${idft_list[@]}"; do
            dir=$([ "$idft" = 1 ] && echo IDFT || echo DFT)
            case_dir="$OUT_DIR/cases/$points/$dir/$short"
            mkdir -p "$case_dir"
            prefix="$case_dir/fft"
            echo "[fft-perf] $short n$points $dir repeat=$repeat"
            if ! build_fft_case "$impl" "$points" "$idft" 0 "$repeat" "$warmup" "$CORPUS" "$prefix" \
                    > "$case_dir/build.log" 2>&1; then
                printf '%s\t%s\t%s\t0\t%s\t0\n' "$impl" "$points" "$idft" "$repeat" >> "$OUT_DIR/raw.tsv"
                fail=$((fail + 1)); continue
            fi
            if ! "$ROOT/tests/common/board_run.sh" "$prefix.bin" "$case_dir/run.log" "$CAPSEC"; then
                printf '%s\t%s\t%s\t0\t%s\t0\n' "$impl" "$points" "$idft" "$repeat" >> "$OUT_DIR/raw.tsv"
                fail=$((fail + 1)); continue
            fi
            line=$(grep -m1 "FFT_ROI cycle=" "$case_dir/run.log" || true)
            cycle=$(sed -n 's/.*cycle=\([0-9]*\).*/\1/p' <<< "$line")
            rep=$(sed -n 's/.*repeat=\([0-9]*\).*/\1/p' <<< "$line")
            cycle=${cycle:-0}; rep=${rep:-0}
            avg=0
            [ "$rep" -gt 0 ] 2>/dev/null && avg=$((cycle / rep))
            printf '%s\t%s\t%s\t%s\t%s\t%s\n' "$impl" "$points" "$idft" "$cycle" "$rep" "$avg" >> "$OUT_DIR/raw.tsv"
            printf 'FFT_ROI cycle=%s repeat=%s average=%s\n' "$cycle" "$rep" "$avg" > "$case_dir/result.txt"
            [ "$avg" -gt 0 ] || fail=$((fail + 1))
        done
    done
done

{
    echo "FFT PERFORMANCE TEST SUMMARY (FPGA)"
    printf '%-14s %-10s %-12s %-10s %-6s\n' CONFIGURATION RVV RVV_EXT REDUCTION STATUS
    for points in "${point_list[@]}"; do
        for idft in "${idft_list[@]}"; do
            dir=$([ "$idft" = 1 ] && echo IDFT || echo DFT)
            rvv=$(awk -F'\t' -v p="$points" -v i="$idft" '$1=="dft_rvv" && $2==p && $3==i {print $6}' "$OUT_DIR/raw.tsv")
            ext=$(awk -F'\t' -v p="$points" -v i="$idft" '$1=="dft_ext" && $2==p && $3==i {print $6}' "$OUT_DIR/raw.tsv")
            if [ -z "$rvv" ] || [ -z "$ext" ] || [ "$rvv" -eq 0 ]; then
                printf '%-14s %-10s %-12s %-10s %-6s\n' "$points-$dir" "${rvv:-NA}" "${ext:-NA}" NA FAIL
                fail=$((fail + 1)); continue
            fi
            reduction=$(awk -v a="$rvv" -v b="$ext" 'BEGIN{printf "%.2f", (a-b)*100.0/a}')
            status=PASS
            awk -v r="$reduction" 'BEGIN{exit !(r < 30)}' && status=FAIL
            [ "$status" = FAIL ] && fail=$((fail + 1))
            printf '%-14s %-10s %-12s %-10s %-6s\n' "$points-$dir" "$rvv" "$ext" "$reduction%" "$status"
        done
    done
    total=$(( ${#point_list[@]} * ${#idft_list[@]} ))
    printf '\ntotal_configurations=%s threshold=30%% status=%s\n' "$total" "$([ "$fail" -eq 0 ] && echo PASS || echo FAIL)"
} > "$OUT_DIR/summary.log"
cat "$OUT_DIR/summary.log"

[ "$fail" -eq 0 ]
