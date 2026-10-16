#!/usr/bin/env bash
# 07 riscv64 交叉工具链 + python3-xlrd -> tools/sysroot  (本机无 sudo, 全部本地化安装)
#   来源: Ubuntu 官方 deb (noble/24.04): 经 apt-get download 缓存在 tools/setup/cache,
#         再用 dpkg-deb -x 解包到 tools/sysroot (无 root 权限亦可)。
#   用途: sw/baremetal 交叉编译 (hello/wtest/probe) 与 hw 引脚表脚本 (xlrd 读 xls)。
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
CACHE="$ROOT/tools/setup/cache"
SYSROOT="$ROOT/tools/sysroot"
mkdir -p "$CACHE" "$SYSROOT"

# 版本以 Ubuntu 24.04 (noble) 为准 (与原始构建一致: gcc 13.3.0 / binutils 2.42)
. /etc/os-release 2>/dev/null || true
if [ "${VERSION_CODENAME:-}" != "noble" ]; then
    echo "[07] 提示: 当前发行版 ${VERSION_CODENAME:-未知} 非 noble, 将安装本发行版对应的交叉工具链"
fi

PKGS=(
    binutils-common
    binutils-riscv64-linux-gnu
    cpp-13-riscv64-linux-gnu
    gcc-13-cross-base
    gcc-13-riscv64-linux-gnu
    gcc-13-riscv64-linux-gnu-base
    gcc-riscv64-linux-gnu
    make
    python3-xlrd
)

for p in "${PKGS[@]}"; do
    if ! ls "$CACHE/${p}_"*.deb >/dev/null 2>&1; then
        echo "[07] 下载 $p"
        ( cd "$CACHE" && apt-get download "$p" )
    fi
    for d in "$CACHE/${p}_"*.deb; do
        [ -f "$d" ] && dpkg-deb -x "$d" "$SYSROOT"
    done
done

GCC="$SYSROOT/usr/bin/riscv64-linux-gnu-gcc"
[ -x "$GCC" ] || { echo "[07] ERROR: 交叉编译器缺失 ($GCC)"; exit 1; }
echo "[07] OK: $("$GCC" --version | head -1)"
python3 - <<PYEOF
import sys
sys.path.insert(0, "$SYSROOT/usr/lib/python3/dist-packages")
import xlrd
print("[07] OK: xlrd", xlrd.__version__)
PYEOF
