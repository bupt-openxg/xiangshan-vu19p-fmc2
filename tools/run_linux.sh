#!/usr/bin/env bash
# =============================================================================
# 一键运行官方 Linux 内存镜像（纯 VIO/TCL, 不需要板卡任何按键）
#   镜像不存在时自动从官方地址下载并校验（sw/linux/fetch_image.sh），
#   随后生成"DTB UART 时钟 25MHz 修正版"（本设计实际时钟为 25MHz）
#   用法: tools/run_linux.sh [串口设备]       默认 /dev/ttyUSB0
#   环境变量: SER=.. LOG=.. CAPSEC=..
#   前置: 已烧录比特流（flows/05_program_fpga.sh）
# =============================================================================
set -u
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1
V="${FPGA_BIT_HOME:-$ROOT/build}"
SER="${1:-${SER:-/dev/ttyUSB0}}"
LOG="${LOG:-${BUILD_ROOT:-$ROOT/build}/logs/serial_linux.log}"
CAPSEC="${CAPSEC:-40}"
IMG_SRC="$ROOT/sw/linux/data.txt"
IMG="$ROOT/sw/linux/data_patched.txt"

[ -e "$SER" ] || { echo "错误: 串口 $SER 不存在"; echo "  甄别: udevadm info -q property -n /dev/ttyUSB0 | grep ID_MODEL  (=FT232R_USB_UART)"; exit 1; }
mkdir -p "$(dirname "$LOG")"

# 0a) 下载官方镜像（如缺）
if [ ! -f "$IMG_SRC" ]; then
    echo "[0/4] 下载官方 Linux 镜像"
    bash "$ROOT/sw/linux/fetch_image.sh" || { echo "错误: 镜像下载失败"; exit 1; }
fi

# 0b) 生成修正版镜像（DTB clock-frequency 50MHz -> 25MHz）
if [ ! -f "$IMG" ] || [ "$IMG_SRC" -nt "$IMG" ]; then
    echo "[0/4] 生成修正版镜像（DTB clock-frequency 50MHz -> 25MHz）"
    python3 "$ROOT/tools/patch_linux_dtb_clock.py" "$IMG_SRC" "$IMG"
else
    echo "[0/4] 使用已有修正版镜像 $IMG"
fi

cd "$FPGA_ROOT"
echo "[1/4] halt_soc  (VIO sw6=0 暂停 CPU)"
make halt_soc FPGA_BIT_HOME="$V" >/dev/null 2>&1

echo "[2/4] write_ddr 写入 13.3MB 镜像（约 55 秒）"
make write_ddr WORKLOAD="$IMG" 2>&1 | grep -E "100%|Error|ERROR" | tail -1

echo "[3/4] 串口 $SER @115200 抓包 -> $LOG"
fuser -k "$SER" >/dev/null 2>&1
sleep 0.5
stty -F "$SER" 115200 cs8 -cstopb -parenb -crtscts raw
timeout "$CAPSEC" cat "$SER" > "$LOG" &
CAP=$!
sleep 1

echo "[4/4] reset_cpu  (VIO sw5 上升沿释放 CPU)"
make reset_cpu FPGA_BIT_HOME="$V" >/dev/null 2>&1
sleep 12
kill $CAP >/dev/null 2>&1

echo "=== 启动日志（$LOG, 尾部 20 行）==="
tail -20 "$LOG" | cat -v
echo "====================================="
echo "若见到 '#' 提示符即启动完成（buildroot root shell）。"
echo "交互:  printf 'uname -a\\r\\n' > $SER   （日志滚动时勿连写, 会 input overrun）"
