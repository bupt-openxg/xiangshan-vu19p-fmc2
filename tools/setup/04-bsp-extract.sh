#!/usr/bin/env bash
# 04 解压 PHINE P6 BSP -> board/PD-VU19P-Solo/ (Pinlist XDC / UG / AEConsole / BSP zip)
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
PKG="$ROOT/board/PHINE.2024.02.P6.vu19p.tar.gz"
DEST="$ROOT/board"
[ -d "$DEST/PD-VU19P-Solo" ] && { echo "[04] 已解压, 跳过"; exit 0; }
if [ ! -f "$PKG" ]; then
    echo "[04] 跳过: 未找到厂商 BSP 包 $PKG"
    echo "      (可选: 从 PHINE 获取 PHINE.2024.02.P6.vu19p.tar.gz 放到 board/ 下;"
    echo "       仅 AEConsole 时钟/下载配置与引脚溯源需要, JTAG 烧录流程不依赖它)"
    exit 0
fi
mkdir -p "$DEST"
tar -xzf "$PKG" -C "$DEST"
echo "[04] OK:"; ls "$DEST/PD-VU19P-Solo"
