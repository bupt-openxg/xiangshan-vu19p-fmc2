#!/usr/bin/env bash
# 06 bin2fpgadata (bin -> data.txt 转换工具, XiangShan-doc 资源) -> tools/bin/
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
CACHE="$ROOT/tools/setup/cache"
URL="https://raw.githubusercontent.com/OpenXiangShan/XiangShan-doc/main/docs/integration/resources/bin2fpgadata.tar.gz"
mkdir -p "$CACHE" "$ROOT/tools/bin"
[ -f "$CACHE/bin2fpgadata.tar.gz" ] || curl -L --fail -o "$CACHE/bin2fpgadata.tar.gz" "$URL"
mkdir -p "$CACHE/bin2fpgadata"
tar -xzf "$CACHE/bin2fpgadata.tar.gz" -C "$CACHE/bin2fpgadata"
# 若含预编译二进制则直接链接; 否则留源码待编译 (自编译裸机程序时才需要)
BIN="$(find "$CACHE/bin2fpgadata" -type f -name "bin2fpgadata*" ! -name "*.c" ! -name "*.cpp" ! -name "*.tar.gz" | head -1)"
# 相对软链 (可移植; 绝对链在 clone 到其它路径后会断)
[ -n "$BIN" ] && ln -sfr "$BIN" "$ROOT/tools/bin/bin2fpgadata"
echo "[06] OK: $(ls -la "$ROOT/tools/bin/" | tail -n +2)"
