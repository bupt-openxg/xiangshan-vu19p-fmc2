#!/usr/bin/env bash
# 00 主机基础依赖: 本机无 sudo, 全部本地化安装到 tools/
#   - make (从 Ubuntu noble .deb 解包到 tools/sysroot, 链接进 tools/bin)
#   - 校验 gcc/git/python3/curl 已存在
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
CACHE="$ROOT/tools/setup/cache"
mkdir -p "$CACHE" "$ROOT/tools/bin" "$ROOT/tools/sysroot"

if ! command -v make >/dev/null && [ ! -x "$ROOT/tools/bin/make" ]; then
    ( cd "$CACHE" && apt-get download make )
    dpkg-deb -x "$CACHE"/make_*.deb "$ROOT/tools/sysroot"
    ln -sf ../sysroot/usr/bin/make "$ROOT/tools/bin/make"
fi
echo "[00] make: $("$ROOT/tools/bin/make" --version | head -1)"
for t in gcc git python3 curl; do command -v "$t" >/dev/null || { echo "[00] 缺少 $t"; exit 1; }; done
echo "[00] OK"
