#!/usr/bin/env bash
# =============================================================================
# 在板卡上运行 BUPT 负载矩阵（48 FFT + 12 MIMO），逐镜像抓串口并判定
#   前置: 新 bitstream 已烧录；SER 默认 /dev/ttyUSB0
#   FPGA_BIT_HOME 指向含 fpga_top_debug.ltx 的目录
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OUT="${BUPT_OUT:-$ROOT/build/bupt-$(date +%Y%m%d)}"
IMG="$OUT/images"
MANIFEST="$OUT/images.tsv"
LOGDIR="$OUT/logs"
SUMMARY="$OUT/summary.tsv"
CAPSEC="${BUPT_CAPSEC:-180}"
SER="${SER:-/dev/ttyUSB0}"

[ -f "$MANIFEST" ] || { echo "ERROR: missing manifest $MANIFEST; run tests/bupt/build.sh" >&2; exit 1; }
[ -e "$SER" ] || { echo "ERROR: serial $SER not found" >&2; exit 1; }
mkdir -p "$LOGDIR"
printf 'kind\tkey\tstatus\tdetail\n' > "$SUMMARY"

fail=0
while IFS=$'\t' read -r kind key bin; do
    [ -n "$key" ] || continue
    log="$LOGDIR/$key.log"
    echo "[bupt] $kind $key"
    if ! "$ROOT/tests/common/board_run.sh" "$bin" "$log" "$CAPSEC"; then
        printf '%s\t%s\tRUN_FAIL\t-\n' "$kind" "$key" >> "$SUMMARY"
        fail=$((fail + 1)); continue
    fi
    if [[ "$kind" == fft ]]; then
        status=PASS
        grep -q "CHECK: PASS" "$log" || status=FAIL
        if [[ "$key" == *ext* ]]; then
            grep -q "EXT instruction demo: PASS" "$log" || status=FAIL
        fi
        detail=$(grep -m1 "CHECK:" "$log" | tr '\t' ' ' || true)
    else
        status=PASS
        grep -q "MIMO_VALIDATE_PASS" "$log" || status=FAIL
        grep -q "MIMO_DONE status=0" "$log" || status=FAIL
        detail=$(grep -m1 "MIMO_DONE" "$log" | tr '\t' ' ' || true)
    fi
    grep -qE "CHECK: FAIL|MIMO_VALIDATE_FAIL|Mismatch" "$log" && status=FAIL
    [ "$status" = PASS ] || fail=$((fail + 1))
    printf '%s\t%s\t%s\t%s\n' "$kind" "$key" "$status" "${detail:-}" >> "$SUMMARY"
done < "$MANIFEST"

total=$(awk -F'\t' 'NR>1{n++} END{print n+0}' "$SUMMARY")
passed=$(awk -F'\t' 'NR>1 && $3=="PASS"{n++} END{print n+0}' "$SUMMARY")
echo "BUPT_RESULT total=$total pass=$passed fail=$((total-passed))"
[ "$fail" -eq 0 ]
