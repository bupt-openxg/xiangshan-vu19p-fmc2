#!/usr/bin/env bash
# =============================================================================
# 08 xPack riscv-none-elf GCC 15.2.0 -> tools/xpack-riscv-none-elf-gcc-15.2.0-1
#   用途: nexus-am 的 FFT/MIMO 工作负载。上游冻结代码使用 GCC 14+ 的 RVV
#         intrinsic 签名（如 vssra 的显式 vxrm 参数），本仓库的 GCC 13.3 无法编译。
#   来源: https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack (GCC 15.2.0-1)
#   全部本地化安装, 无需 sudo; tarball 缓存在 tools/setup/cache (gitignored)。
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
CACHE="$ROOT/tools/setup/cache"
VER="15.2.0-1"
NAME="xpack-riscv-none-elf-gcc-${VER}"
DEST="$ROOT/tools/$NAME"
mkdir -p "$CACHE"

if [ -x "$DEST/bin/riscv-none-elf-gcc" ]; then
    echo "[08] 已安装: $("$DEST/bin/riscv-none-elf-gcc" --version | head -1)"
    exit 0
fi

TAR="$CACHE/${NAME}-linux-x64.tar.gz"
URL="https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/download/v${VER}/${NAME}-linux-x64.tar.gz"
if [ ! -f "$TAR" ]; then
    echo "[08] 下载 $URL（约 413MB）"
    curl -fL --retry 3 --retry-all-errors -o "$TAR.part" "$URL"
    mv "$TAR.part" "$TAR"
fi
echo "[08] 解包到 tools/"
tar xzf "$TAR" -C "$ROOT/tools"

[ -x "$DEST/bin/riscv-none-elf-gcc" ] || {
    echo "[08] ERROR: 解包后未找到 $DEST/bin/riscv-none-elf-gcc"; exit 1; }
echo "[08] OK: $("$DEST/bin/riscv-none-elf-gcc" --version | head -1)"
