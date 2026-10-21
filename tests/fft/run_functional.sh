#!/usr/bin/env bash
# =============================================================================
# 测试用例二 / 流程一：FFT 功能验证（7 点 × 2 方向 × 5 输入 × 2 实现 = 140）
#   实现: dft_rvv（RVV）与 dft_ext（RVV+扩展）
#   判定: 每项 FFT_EXP_VALIDATION mismatching_components=0 且 CHECK: PASS
#   说明: 官方脚本用 NEMU；本脚本把同一裸机镜像放到 FPGA 上运行。
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
source "$ROOT/tests/common/build_env.sh"

OUT_DIR="$OUT/fft-functional"
mkdir -p "$OUT_DIR"
POINTS="${FFT_TEST_POINTS:-64,128,256,512,1024,2048,4096}"
IDFTS="${FFT_TEST_IDFTS:-0,1}"
CORPORA="${FFT_TEST_CORPORA:-C0-legacy-pattern-v1,C1-impulse-v1,C2-dc-alternating-v1,C3-prng-q15-v1,C4-near-limit-v1}"
IMPLS="${FFT_TEST_IMPLS:-dft_rvv,dft_ext}"
CAPSEC="${FFT_CAPSEC:-90}"

printf 'impl\tpoints\tidft\tcorpus\tstatus\tmismatch\tcheck\n' > "$OUT_DIR/cases.tsv"

IFS=',' read -r -a point_list <<< "$POINTS"
IFS=',' read -r -a idft_list <<< "$IDFTS"
IFS=',' read -r -a corpus_list <<< "$CORPORA"
IFS=',' read -r -a impl_list <<< "$IMPLS"

fail=0
total=0
for impl in "${impl_list[@]}"; do
    short=${impl#dft_}
    for points in "${point_list[@]}"; do
        for idft in "${idft_list[@]}"; do
            dir=$([ "$idft" = 1 ] && echo IDFT || echo DFT)
            for corpus in "${corpus_list[@]}"; do
                total=$((total + 1))
                case_dir="$OUT_DIR/cases/$points/$dir/$corpus/$short"
                mkdir -p "$case_dir"
                prefix="$case_dir/fft"
                echo "[fft-func] $short n$points $dir $corpus"
                if ! build_fft_case "$impl" "$points" "$idft" 1 1 1 "$corpus" "$prefix" \
                        > "$case_dir/build.log" 2>&1; then
                    printf '%s\t%s\t%s\t%s\tFAIL\t-\t-\n' "$impl" "$points" "$idft" "$corpus" >> "$OUT_DIR/cases.tsv"
                    fail=$((fail + 1)); continue
                fi
                if ! "$ROOT/tests/common/board_run.sh" "$prefix.bin" "$case_dir/run.log" "$CAPSEC"; then
                    printf '%s\t%s\t%s\t%s\tFAIL\t-\t-\n' "$impl" "$points" "$idft" "$corpus" >> "$OUT_DIR/cases.tsv"
                    fail=$((fail + 1)); continue
                fi
                line=$(grep -m1 "FFT_EXP_VALIDATION corpus=$corpus points=$points " "$case_dir/run.log" || true)
                mismatch=$(sed -n 's/.*mismatching_components=\([0-9]*\).*/\1/p' <<< "$line")
                mismatch=${mismatch:-1}
                check=FAIL
                grep -q "CHECK: PASS" "$case_dir/run.log" && check=PASS
                status=PASS
                if [ "$mismatch" != 0 ] || [ "$check" != PASS ] || \
                   grep -qE "CHECK: FAIL|Mismatch at" "$case_dir/run.log"; then
                    status=FAIL; fail=$((fail + 1))
                fi
                if [ "$impl" = dft_ext ] && ! grep -q "EXT instruction demo: PASS" "$case_dir/run.log"; then
                    status=FAIL; fail=$((fail + 1))
                fi
                printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\n' "$impl" "$points" "$idft" "$corpus" "$status" "$mismatch" "$check" >> "$OUT_DIR/cases.tsv"
                grep -m1 "FFT_EXP_VALIDATION corpus=$corpus points=$points " "$case_dir/run.log" > "$case_dir/result.txt" || true
                grep -m1 "FFT_EXP_VALIDATION corpus=$corpus points=$points " "$case_dir/run.log" > "$case_dir/result.txt" || true
            done
        done
    done
done

summary_status=$([ "$fail" -eq 0 ] && echo PASS || echo FAIL)
{
    echo "FFT FUNCTIONAL TEST SUMMARY (FPGA)"
    printf '%-18s %-8s %-8s %-8s\n' CONFIGURATION RVV RVV_EXT STATUS
    for points in "${point_list[@]}"; do
        for idft in "${idft_list[@]}"; do
            dir=$([ "$idft" = 1 ] && echo IDFT || echo DFT)
            rvv=$(awk -F'\t' -v p="$points" -v i="$idft" '$1=="dft_rvv" && $2==p && $3==i && $5=="PASS" {n++} END{print n+0}' "$OUT_DIR/cases.tsv")
            ext=$(awk -F'\t' -v p="$points" -v i="$idft" '$1=="dft_ext" && $2==p && $3==i && $5=="PASS" {n++} END{print n+0}' "$OUT_DIR/cases.tsv")
            row=PASS; [ "$rvv" -eq 5 ] && [ "$ext" -eq 5 ] || row=FAIL
            printf '%-18s %-8s %-8s %-8s\n' "$points-$dir" "$rvv/5" "$ext/5" "$row"
        done
    done
    printf '\ntotal_cases=%s pass=%s fail=%s status=%s\n' "$total" "$((total - fail))" "$fail" "$summary_status"
} | tee "$OUT_DIR/summary.log"

[ "$fail" -eq 0 ]
