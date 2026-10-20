#!/usr/bin/env bash
# =============================================================================
# 09 构建 XVBBP 裸机负载（官方冻结 FFT/MIMO，nexus-am）
#   产物: build/xvbbp/<key>.bin + build/xvbbp/images.tsv（供 flows/10 使用）
#   默认构建: smoke(7 指令冒烟) + FFT ext/scalar 代表尺寸 + MIMO ext/scalar 代表内核
#   ALL=1: 额外跑官方 xvbbp-build-all（48 FFT + 12 MIMO，仅构建不归档）
#   工具链: tools/setup/08 安装的 xPack GCC 15（GCC 13 不支持上游 RVV intrinsic 签名）
#   板级: 必须 -DUART16550=1（本板 16550 @0x310b0000）;
#         FFT 的 OAI 校验参考编成 RV64GC（patches/nexus-am/0004，避免 SIMDe RVV 卡死）
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true

AM="$ROOT/third_party/nexus-am"
PKG="$ROOT/third_party/riscv-xvbbp"
OUT="$BUILD_ROOT/xvbbp"
CROSS="${RISCV_XPACK_PREFIX:-}"

[ -d "$AM/.git" ] || { echo "ERROR: 缺少 $AM（先运行 flows/02_clone_deps.sh）"; exit 1; }
[ -d "$PKG/software/nexus-am/apps/fft" ] || { echo "ERROR: 缺少 $PKG（先运行 flows/02_clone_deps.sh）"; exit 1; }
[ -n "$CROSS" ] && [ -x "${CROSS}gcc" ] || {
    echo "ERROR: 未找到 xPack GCC 15（FFT/MIMO 的 RVV intrinsic 需要 GCC14+）"
    echo "  安装: tools/setup/08-xpack-riscv-gcc.sh"
    exit 1
}
mkdir -p "$OUT"
MANIFEST="$OUT/images.tsv"
: > "$MANIFEST"

# ---- 安装官方 apps（nexus-am 树为 gitignored 第三方源码，每次从冻结包覆盖）----
for app in fft mimo; do
    rm -rf "$AM/apps/$app"
    mkdir -p "$AM/apps/$app"
    cp -a "$PKG/software/nexus-am/apps/$app/." "$AM/apps/$app/"
done

# 参考实现标量化（SIMDe 的 RVV dft16 在本核上重复调用会卡死）
PATCH_REF="$ROOT/patches/nexus-am/0004-fft-oai-scalar-reference.patch"
[ -f "$PATCH_REF" ] || { echo "ERROR: 缺少 $PATCH_REF"; exit 1; }
patch -p1 -d "$AM/apps" --batch < "$PATCH_REF" || {
    echo "ERROR: 应用 $PATCH_REF 失败"; exit 1;
}

# UART16550 宏未纳入 Makefile 依赖: 切换串口后端时必须手工清掉 am.a
rm -rf "$AM/am/build"

COMMON=(
    ARCH=riscv64-xs
    AM_HOME="$AM"
    CROSS_COMPILE="$CROSS"
    RVV_TOOLCHAIN_PREFIX="$CROSS"
    # 本板是 16550 @0x310b0000; 默认的 uartlite @0x40600000 会卡死 CPU
    "CPPFLAGS=-DUART16550=1"
    "CC_OPT=-O2 -Wno-error=unused-variable -Wno-error=unused-function"
)

record() { printf '%s\t%s\t%s\n' "$1" "$2" "$3" >> "$MANIFEST"; }

build_fft() { # key impl points idft
    local key=$1 impl=$2 points=$3 idft=$4
    echo "[09] FFT $impl n$points idft=$idft -> $key"
    make -C "$AM/apps/fft" DFT_IMPL="$impl" FFT_POINTS="$points" FFT_RUN_IDFT="$idft" \
         FFT_VALIDATE=1 FFT_REPEAT=1 "${COMMON[@]}"
    local name="hello-${impl#dft_}-validate-n${points}-r1-idft${idft}-riscv64-xs.bin"
    local bin="$AM/apps/fft/build/$name"
    [ -f "$bin" ] || { echo "ERROR: 未找到 $bin"; exit 1; }
    cp -f "$bin" "$OUT/$key.bin"
    record "$key" "$OUT/$key.bin" "FFT $impl n$points idft=$idft"
}

build_mimo() { # key impl kernel
    local key=$1 impl=$2 kernel=$3
    echo "[09] MIMO $impl $kernel -> $key"
    make -C "$AM/apps/mimo" MIMO_IMPL="$impl" MIMO_VALIDATE_KERNEL="$kernel" MIMO_PERF=0 "${COMMON[@]}"
    local bin="$AM/apps/mimo/build/mimo-${impl}.bin"
    [ -f "$bin" ] || { echo "ERROR: 未找到 $bin"; exit 1; }
    cp -f "$bin" "$OUT/$key.bin"
    record "$key" "$OUT/$key.bin" "MIMO $impl $kernel"
}

# ---- 默认代表集 ----
build_fft smoke           dft_ext    16   0   # 官方 7 指令冒烟（打印 EXT instruction demo: PASS）
build_fft fft-ext-1024    dft_ext    1024 0
build_fft fft-scalar-1024 dft_scalar 1024 0
build_fft fft-ext-4096    dft_ext    4096 0
build_mimo mimo-ext-zf4    ext    eq_zf_4x4
build_mimo mimo-ext-mmse2  ext    eq_mmse_2x2
build_mimo mimo-scalar-zf4 scalar eq_zf_4x4

if [ "${ALL:-0}" = 1 ]; then
    echo "[09] 官方全量构建: 48 FFT + 12 MIMO（不逐镜像归档）"
    make -C "$AM/apps/fft" xvbbp-build-all "${COMMON[@]}"
    make -C "$AM/apps/mimo" xvbbp-build-all "${COMMON[@]}"
fi

echo
echo "==== XVBBP 负载就绪（$OUT）===="
while IFS=$'\t' read -r key path desc; do
    printf '  %-18s %9s  %s\n' "$key" "$(stat -c %s "$path")" "$desc"
done < "$MANIFEST"
echo "下一步: flows/10_run_xvbbp.sh smoke [串口]"
