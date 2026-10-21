#!/usr/bin/env bash
# =============================================================================
# 测试用例一 / 流程二：KMHv2 整核给定激励测试（52 用例）
#   官方源码: nexus-am/apps/ext-instruction-test @ kunminghu-v2-xvbbp-test
#   运行环境: PHINE PD-VU19P-2-LSI SOLO v3（裸机, UART 115200）
#   判定: SUMMARY: 52/52 cases passed + EXT instruction fixed-vector test: PASS
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
source "$ROOT/tests/common/build_env.sh"

OUT_DIR="$OUT/ext-instruction-test"
BIN="$OUT_DIR/ext-instruction-test.bin"
LOG="$OUT_DIR/run.log"
mkdir -p "$OUT_DIR"

echo "[ext] build"
build_ext_test "$BIN" > "$OUT_DIR/build.log" 2>&1
echo "[ext] run on FPGA"
"$ROOT/tests/common/board_run.sh" "$BIN" "$LOG" 90

pass=0
grep -q "SUMMARY: 52/52 cases passed" "$LOG" && \
grep -q "EXT instruction fixed-vector test: PASS" "$LOG" && pass=1
grep -qE "FAIL|Mismatch|mismatch" "$LOG" && pass=0

{
    echo "EXT_INSTRUCTION_FIXED_VECTOR_TEST"
    echo "cases=52"
    echo "log=$LOG"
    if [ "$pass" = 1 ]; then
        echo "status=PASS"
        echo "summary=SUMMARY: 52/52 cases passed"
        echo "verdict=EXT instruction fixed-vector test: PASS"
    else
        echo "status=FAIL"
    fi
} | tee "$OUT_DIR/summary.log"

[ "$pass" = 1 ]
