#!/usr/bin/env bash
# =============================================================================
# 一键运行裸机 Hello World（纯 VIO/TCL, 不需要板卡任何按键）
#   用法: tools/run_hello.sh [串口设备]       默认 /dev/ttyUSB0
#   环境变量: SER=/dev/ttyUSB1 LOG=/tmp/xx.log CAPSEC=45（需大于 reset_cpu 的启动耗时）
#   前置: 已烧录比特流（flows/05_program_fpga.sh）；sw/baremetal/hello.txt 已构建
# =============================================================================
set -u
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1
V="${FPGA_BIT_HOME:-$ROOT/build}"
SER="${1:-${SER:-/dev/ttyUSB0}}"
LOG="${LOG:-${BUILD_ROOT:-$ROOT/build}/logs/serial_hello.log}"
CAPSEC="${CAPSEC:-45}"
WL="${WORKLOAD:-$ROOT/sw/baremetal/hello.txt}"

[ -f "$WL" ] || { echo "错误: 找不到工作负载 $WL"; echo "  构建: source tools/env.sh && make -C sw/baremetal"; exit 1; }
[ -e "$SER" ] || { echo "错误: 串口 $SER 不存在"; echo "  甄别: udevadm info -q property -n /dev/ttyUSB0 | grep ID_MODEL  (=FT232R_USB_UART)"; exit 1; }
mkdir -p "$(dirname "$LOG")"

cd "$FPGA_ROOT"
echo "[1/4] halt_soc  (VIO sw6=0 暂停 CPU)"
make halt_soc FPGA_BIT_HOME="$V" >/dev/null 2>&1

echo "[2/4] write_ddr 写入 $WL"
make write_ddr WORKLOAD="$WL" 2>&1 | grep -E "100%|Error|ERROR" | tail -1

echo "[3/4] 串口 $SER @115200 抓包 ${CAPSEC}s -> $LOG"
fuser -k "$SER" >/dev/null 2>&1
sleep 0.5
stty -F "$SER" 115200 cs8 -cstopb -parenb -crtscts raw
timeout "$CAPSEC" cat "$SER" > "$LOG" &
CAP=$!
sleep 1

echo "[4/4] reset_cpu  (VIO sw5 上升沿释放 CPU)"
make reset_cpu FPGA_BIT_HOME="$V" >/dev/null 2>&1
wait $CAP 2>/dev/null

echo "=== 串口输出（$LOG）==="
cat -v "$LOG"
echo "==========================="
echo "期望: Hello, XiangShan! (MinimalConfig on PHINE PD-VU19P, 115200 8N1)"
