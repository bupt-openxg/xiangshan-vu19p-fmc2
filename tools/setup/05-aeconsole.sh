#!/usr/bin/env bash
# 05 安装 AEConsole (Ubuntu 版, 来自 BSP Software/AEConsole/Ubuntu) -> tools/aeconsole/
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
SRC_DIR="$ROOT/board/PD-VU19P-Solo/Software/AEConsole/Ubuntu"
DEST="$ROOT/tools/aeconsole"
if [ ! -d "$SRC_DIR" ]; then
    echo "[05] 跳过: 未找到 AEConsole 源 ($SRC_DIR)"
    echo "      (可选: 需先放置厂商 BSP 到 board/ 并运行 04; 纯 JTAG 流程不依赖 AEConsole)"
    exit 0
fi
mkdir -p "$DEST"
if [ -z "$(ls -A "$DEST" 2>/dev/null)" ]; then
    tar -xzf "$SRC_DIR"/ae-console-*.tar.gz -C "$DEST"
fi
echo "[05] OK:"; ls "$DEST" | head
