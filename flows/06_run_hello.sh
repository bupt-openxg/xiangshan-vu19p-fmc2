#!/usr/bin/env bash
# =============================================================================
# 06 运行裸机 Hello World（全 TCL/VIO 控制, 不使用板卡任何按键）
#   步骤: 构建负载(如需) → halt → 写入 DDR → 串口抓包 → reset_cpu → 打印结果
#   用法: flows/06_run_hello.sh [串口设备]     默认 /dev/ttyUSB0
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true

if [ ! -f "$ROOT/sw/baremetal/hello.txt" ]; then
    echo "[06] 构建裸机负载（hello.txt）"
    make -C "$ROOT/sw/baremetal" hello.txt
fi

exec bash "$ROOT/tools/run_hello.sh" "$@"
