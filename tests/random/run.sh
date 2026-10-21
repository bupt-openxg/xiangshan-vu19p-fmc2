#!/usr/bin/env bash
# =============================================================================
# 测试用例一 / 流程一（FPGA 可执行版本）：7 条扩展指令随机激励
#   逐指令、逐 lane 与 C 标量参考模型比对，vxrm 四种模式全遍历。
#   注: 文档的 10^9 条功能单元级随机测试依赖 YunSuan emu；板卡上运行核级
#       随机测试，规模默认为 1,000,000 条指令请求（可用 RANDOM_CASES 调大）。
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
source "$ROOT/tests/common/build_env.sh"

OUT_DIR="$OUT/random"
mkdir -p "$OUT_DIR"
CASES="${RANDOM_CASES:-1000000}"
BIN="$OUT_DIR/ext-random.bin"
LOG="$OUT_DIR/run.log"
# RANDOM_CASES 只进 CFLAGS，Make 不跟踪；按规模分目录，避免复用旧对象
OBJ_DIR="$OUT_DIR/objects/cases-$CASES"
mkdir -p "$OBJ_DIR"

echo "[random] build cases=$CASES"
make -C "$ROOT/tests/random" \
    "${COMMON[@]}" CPPFLAGS=-DUART16550=1 \
    RANDOM_CASES="$CASES" \
    DST_DIR="$OBJ_DIR/" BINARY="${BIN%.bin}" -j1 \
    > "$OUT_DIR/build.log" 2>&1

echo "[random] run on FPGA"
"$ROOT/tests/common/board_run.sh" "$BIN" "$LOG" 240

pass=0
grep -q "XVBBP_RANDOM_PASS cases=$CASES" "$LOG" && pass=1
grep -q "XVBBP_RANDOM_FAIL" "$LOG" && pass=0

{
    echo "XVBBP EXT RANDOM STIMULUS (FPGA)"
    echo "cases=$CASES ops=7 vxrm=0..3 sew=e16 vl=8"
    echo "log=$LOG"
    echo "status=$([ "$pass" = 1 ] && echo PASS || echo FAIL)"
} > "$OUT_DIR/summary.log"
cat "$OUT_DIR/summary.log"
cp "$LOG" "$OUT_DIR/run-$CASES.log"
cp "$OUT_DIR/summary.log" "$OUT_DIR/summary-$CASES.log"

[ "$pass" = 1 ]
