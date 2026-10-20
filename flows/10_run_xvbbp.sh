#!/usr/bin/env bash
# =============================================================================
# 10 运行 XVBBP 裸机负载（纯 VIO/TCL, 不使用板卡任何按键）
#   用法: flows/10_run_xvbbp.sh [key|image.bin] [串口]     默认 key=smoke
#   key 来自 build/xvbbp/images.tsv（flows/09 生成）:
#     smoke / fft-ext-1024 / fft-scalar-1024 / fft-ext-4096 /
#     mimo-ext-zf4 / mimo-ext-mmse2 / mimo-scalar-zf4
#   也可直接传 image.bin（flows/11 构建的 am-bench 测试）:
#     dhrystone / coremark-2iter / microbench-test
#   环境变量: SER=/dev/ttyUSB1  LOG=...  CAPSEC=300（MIMO/FFT 可能需要更久）
#   前置: 已烧录比特流（flows/05_program_fpga.sh）
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true

V="${FPGA_BIT_HOME:-$ROOT/build}"
KEY="${1:-smoke}"
SER="${2:-${SER:-/dev/ttyUSB0}}"
CAPSEC="${CAPSEC:-300}"
OUT="$BUILD_ROOT/xvbbp"
MANIFEST="$OUT/images.tsv"

if [ -f "$KEY" ]; then
    # 绝对化: 后面会 cd 到 FPGA_ROOT，相对路径会失效
    BIN="$(cd "$(dirname "$KEY")" && pwd)/$(basename "$KEY")"
    KEY="$(basename "$KEY" .bin)"
else
    [ -f "$MANIFEST" ] || { echo "ERROR: 缺少 $MANIFEST（先运行 flows/09_build_xvbbp_sw.sh）"; exit 1; }
    BIN="$(awk -F'\t' -v k="$KEY" '$1==k{print $2}' "$MANIFEST")"
    [ -n "$BIN" ] || {
        echo "ERROR: 未知负载 '$KEY'。可用："
        awk -F'\t' '{printf "  %s\n", $1}' "$MANIFEST"
        exit 1
    }
fi
[ -f "$BIN" ] || { echo "ERROR: 镜像不存在: $BIN"; exit 1; }
[ -e "$SER" ] || { echo "ERROR: 串口 $SER 不存在（udevadm info -q property -n $SER | grep ID_MODEL）"; exit 1; }

mkdir -p "$OUT" "$BUILD_ROOT/logs"
LOG="${LOG:-$BUILD_ROOT/logs/xvbbp-${KEY}-$(date +%Y%m%d-%H%M).log}"
TXT="$OUT/$KEY.txt"
"$TOOLS_ROOT/bin/bin2fpgadata" -i "$BIN" -o "$TXT"

cd "$FPGA_ROOT"
echo "[1/4] halt_soc  (VIO sw6=0 暂停 CPU)"
make halt_soc FPGA_BIT_HOME="$V" >/dev/null 2>&1

echo "[2/4] write_ddr 写入 $KEY（$(stat -c %s "$BIN") 字节）"
make write_ddr WORKLOAD="$TXT" 2>&1 | grep -E "100%|Error|ERROR" | tail -1 || true

echo "[3/4] 串口 $SER @115200 抓包 ${CAPSEC}s -> $LOG"
fuser -k "$SER" >/dev/null 2>&1 || true
sleep 0.5
# -echo 必加: GNU stty 的 raw 不清 echo, 开着会把 FPGA 输出回写给 FPGA (input overrun)
stty -F "$SER" 115200 cs8 -cstopb -parenb -crtscts -echo raw
timeout "$CAPSEC" cat "$SER" > "$LOG" &
CAP=$!
sleep 1

echo "[4/4] reset_cpu  (VIO sw5 上升沿释放 CPU)"
make reset_cpu FPGA_BIT_HOME="$V" >/dev/null 2>&1
wait $CAP 2>/dev/null || true

echo "=== 串口输出（$LOG）==="
cat -v "$LOG"
echo "==========================="

fail=0
case "$KEY" in
    smoke)
        grep -q "EXT instruction demo: PASS" "$LOG" || fail=1
        grep -q "CHECK: PASS" "$LOG" || fail=1 ;;
    fft-ext*)
        grep -q "EXT instruction demo: PASS" "$LOG" || fail=1
        grep -q "CHECK: PASS" "$LOG" || fail=1 ;;
    fft-scalar*)
        grep -q "CHECK: PASS" "$LOG" || fail=1 ;;
    mimo*)
        grep -q "MIMO_VALIDATE_PASS" "$LOG" || fail=1
        grep -q "MIMO_DONE" "$LOG" || fail=1 ;;
    dhrystone*)
        grep -q "Dhrystone PASS" "$LOG" || fail=1 ;;
    coremark*)
        grep -q "CoreMark Iterations/Sec" "$LOG" || fail=1
        # 2K performance run 的标准校验值（与迭代次数无关）
        grep -q "crcfinal.*0x72be" "$LOG" || fail=1 ;;
    microbench*)
        grep -q "MicroBench PASS" "$LOG" || fail=1 ;;
esac
grep -qE "Mismatch|EXT instruction demo: FAIL|CHECK: FAIL|MIMO_VALIDATE_FAIL|vxsat mismatch|rotpair demo: FAIL|Dhrystone FAIL|MicroBench FAIL|Validation failed" "$LOG" && fail=1

if [ "$fail" = 0 ]; then
    echo "结果: PASS（$KEY）"
else
    echo "结果: FAIL（$KEY）—— 检查日志与 VIO 探针: vivado -mode tcl -source tools/read_probes.tcl"
    exit 1
fi
