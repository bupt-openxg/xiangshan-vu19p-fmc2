#!/usr/bin/env bash
# =============================================================================
# 12 运行 XVBBP 复杂矩阵（板上）
#   覆盖: EXT FFT n16..n4096 正/逆变换（共 16 个）+ 4 个 MIMO kernel
#         （逆变换与共轭指令在真实 FFT 数据流中生效）
#   前置: BUILD=1 flows/12_run_xvbbp_matrix.sh   # 先构建矩阵（flows/09 MATRIX=1）
#         flows/05_program_fpga.sh
#   产物: build/xvbbp-matrix/logs/<key>.{serial,run}.log + summary.tsv
#   环境变量: BUILD=1 自动构建; FFT_CAPSEC/MIMO_CAPSEC 覆盖抓包时长
# =============================================================================
set -u
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true

OUT="$BUILD_ROOT/xvbbp-matrix"
MANIFEST="$BUILD_ROOT/xvbbp/images.tsv"
FFT_CAPSEC="${FFT_CAPSEC:-40}"
MIMO_CAPSEC="${MIMO_CAPSEC:-120}"

if [ "${BUILD:-0}" = 1 ]; then
    MATRIX=1 bash "$ROOT/flows/09_build_xvbbp_sw.sh"
fi
[ -f "$MANIFEST" ] || { echo "ERROR: 缺少 $MANIFEST（BUILD=1 或先跑 flows/09 MATRIX=1）"; exit 1; }
mkdir -p "$OUT/logs"
: > "$OUT/summary.tsv"

has_key() { awk -F'\t' -v k="$1" '$1==k{found=1} END{exit !found}' "$MANIFEST"; }

fail_total=0
run_one() { # key capsec
    local key=$1 cap=$2 rc verdict
    if ! has_key "$key"; then
        printf '%-22s MISSING（先 MATRIX=1 flows/09）\n' "$key"
        printf '%s\tMISSING\t-\n' "$key" >> "$OUT/summary.tsv"
        fail_total=$((fail_total + 1)); return
    fi
    echo "---- $key (${cap}s) ----"
    LOG="$OUT/logs/${key}.serial.log" CAPSEC="$cap" \
        bash "$ROOT/flows/10_run_xvbbp.sh" "$key" > "$OUT/logs/${key}.run.log" 2>&1
    rc=$?
    verdict="$(grep -aE '^结果:' "$OUT/logs/${key}.run.log" | tail -1)"
    [ -n "$verdict" ] || verdict="NO-VERDICT(rc=$rc)"
    printf '%-22s %s\n' "$key" "$verdict"
    printf '%s\t%s\t%s\n' "$key" "$([ "$rc" = 0 ] && echo PASS || echo FAIL)" "$verdict" >> "$OUT/summary.tsv"
    [ "$rc" = 0 ] || fail_total=$((fail_total + 1))
}

run_one smoke "$FFT_CAPSEC"
for points in 16 64 128 256 512 1024 2048 4096; do
    run_one "fft-ext-n${points}-fwd"  "$FFT_CAPSEC"
    run_one "fft-ext-n${points}-idft" "$FFT_CAPSEC"
done
run_one mimo-ext-zf2   "$MIMO_CAPSEC"
run_one mimo-ext-mmse2 "$((MIMO_CAPSEC * 3 / 4))"
run_one mimo-ext-zf4   "$MIMO_CAPSEC"
run_one mimo-ext-mmse4 "$MIMO_CAPSEC"

echo
echo "==== XVBBP 矩阵结果（$OUT/summary.tsv）===="
column -t -s $'\t' "$OUT/summary.tsv" 2>/dev/null || cat "$OUT/summary.tsv"
if [ "$fail_total" = 0 ]; then
    echo "矩阵: ALL PASS"
else
    echo "矩阵: $fail_total 项失败"
    exit 1
fi
