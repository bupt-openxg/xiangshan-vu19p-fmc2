#!/usr/bin/env bash
# =============================================================================
# 单次上板运行：一次 Vivado 会话完成 halt / 写 DDR / 复位，串口同步抓取
#   用法: tests/common/board_run.sh <image.bin> <log> [capsec]
#   依赖: build/fpga_top_debug.bit 已烧录；SER 默认 /dev/ttyUSB0
#   返回: 0 = 板级启动序列完成（应用判定由调用方解析日志）
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true

BIN="${1:?usage: board_run.sh <image.bin> <log> [capsec]}"
LOG="${2:?usage: board_run.sh <image.bin> <log> [capsec]}"
CAPSEC="${3:-120}"
SER="${SER:-/dev/ttyUSB0}"
LTX="${FPGA_BIT_HOME:-$ROOT/build}/fpga_top_debug.ltx"
V="${FPGA_BIT_HOME:-$ROOT/build}"

[ -f "$BIN" ] || { echo "ERROR: image not found: $BIN" >&2; exit 2; }
[ -e "$SER" ] || { echo "ERROR: serial port not found: $SER" >&2; exit 2; }
[ -f "$LTX" ] || { echo "ERROR: probes not found: $LTX" >&2; exit 3; }

mkdir -p "$(dirname "$LOG")" "$ROOT/build/tests/tmp"
TXT="$ROOT/build/tests/tmp/$(basename "${BIN%.bin}").txt"
BOOTLOG="${LOG%.log}.boot.log"
"$TOOLS_ROOT/bin/bin2fpgadata" -i "$BIN" -o "$TXT" >/dev/null

fuser -k "$SER" >/dev/null 2>&1 || true
sleep 0.3
# 清掉上一次遗留的输入字节；-echo 防止 GNU raw 模式把输出回写给 FPGA
stty -F "$SER" 115200 cs8 -cstopb -parenb -crtscts -echo raw
timeout 0.3 cat "$SER" >/dev/null 2>&1 || true

: > "$LOG"
timeout "$CAPSEC" cat "$SER" > "$LOG" &
CAP=$!

cd "$ROOT"
set +e
vivado -mode batch -notrace -source tests/common/board_boot.tcl \
    -tclargs "$LTX" "$TXT" > "$BOOTLOG" 2>&1
VRC=$?
set -e

# 等待应用结束标记，最多 CAPSEC 秒；超时后仍保留已抓到的日志
deadline=$(( $(date +%s) + CAPSEC ))
while kill -0 "$CAP" 2>/dev/null; do
    if grep -q "Exit with code =" "$LOG" 2>/dev/null; then
        sleep 0.5
        break
    fi
    [ "$(date +%s)" -lt "$deadline" ] || break
    sleep 0.2
done
kill "$CAP" 2>/dev/null || true
wait "$CAP" 2>/dev/null || true

# CRLF -> LF，便于后续解析
tr -d '\r' < "$LOG" > "$LOG.tmp" && mv "$LOG.tmp" "$LOG"

if ! grep -q "BOARD_RESET_DONE" "$BOOTLOG" 2>/dev/null; then
    echo "ERROR: board boot sequence failed (vivado rc=$VRC), see $BOOTLOG" >&2
    tail -20 "$BOOTLOG" >&2 || true
    exit 4
fi
if [ "$VRC" -ne 0 ]; then
    echo "WARNING: vivado rc=$VRC but BOARD_RESET_DONE observed" >&2
fi
exit 0
