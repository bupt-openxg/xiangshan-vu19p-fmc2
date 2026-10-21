#!/usr/bin/env bash
# =============================================================================
# 构建 BUPT riscv-xvbbp 的完整负载矩阵（不做任何自定义测试）
#   FFT: 3 后端 × 8 点 × 2 方向 = 48
#   MIMO: 3 后端 × 4 内核 = 12
#   产物: build/bupt-<date>/images/{fft,mimo} + images.tsv
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true

AM="$ROOT/third_party/nexus-am"
PKG="$ROOT/third_party/riscv-xvbbp"
OUT="${BUPT_OUT:-$ROOT/build/bupt-$(date +%Y%m%d)}"
IMG="$OUT/images"
CROSS="${RISCV_XPACK_PREFIX:?missing xPack RISC-V toolchain}"

[ -d "$PKG/software/nexus-am/apps/fft" ] || {
    echo "ERROR: missing BUPT repo at $PKG" >&2
    exit 1
}
[ -d "$AM" ] || { echo "ERROR: missing $AM" >&2; exit 1; }

COMMON=(
    ARCH=riscv64-xs
    AM_HOME="$AM"
    CROSS_COMPILE="$CROSS"
    RVV_TOOLCHAIN_PREFIX="$CROSS"
    "CPPFLAGS=-DUART16550=1"
    "CC_OPT=-O2 -Wno-error=unused-variable -Wno-error=unused-function"
)

mkdir -p "$IMG/fft" "$IMG/mimo"
: > "$OUT/images.tsv"

echo "[bupt] install apps into official nexus-am"
rm -rf "$AM/apps/fft" "$AM/apps/mimo"
cp -a "$PKG/software/nexus-am/apps/fft" "$AM/apps/"
cp -a "$PKG/software/nexus-am/apps/mimo" "$AM/apps/"

# Keep the OAI validation reference scalar on this FPGA port (SIMDe's SSE dft16
# maps to RVV under -march=rv64gcv and hangs after a few repeated calls).
cat >> "$AM/apps/fft/Makefile" <<'EOF'

# ReConstruct: build the OAI reference scalar; object path matches OBJS.
$(DST_DIR)/./oai_dfts.o: CFLAGS := $(filter-out -march=rv64gc -march=rv64gcv,$(CFLAGS)) -march=rv64gc
EOF

# UART backend macro is not tracked by make; force a clean AM build.
rm -rf "$AM/am/build" "$AM/libs/klib/build"

echo "[bupt] FFT 48 images"
make -C "$AM/apps/fft" xvbbp-build-all "${COMMON[@]}" -j4
for f in "$AM"/apps/fft/build/hello-*-validate-n*-r1-idft*-riscv64-xs.bin; do
    [ -f "$f" ] || continue
    base=$(basename "$f" .bin)
    cp -f "$f" "$IMG/fft/$base.bin"
    printf 'fft\t%s\t%s\n' "$base" "$IMG/fft/$base.bin" >> "$OUT/images.tsv"
done

echo "[bupt] MIMO 12 images"
for impl in scalar rvv ext; do
    for kernel in eq_zf_2x2 eq_mmse_2x2 eq_zf_4x4 eq_mmse_4x4; do
        echo "  mimo $impl/$kernel"
        make -C "$AM/apps/mimo" \
            MIMO_IMPL="$impl" MIMO_VALIDATE_KERNEL="$kernel" MIMO_PERF=0 \
            "${COMMON[@]}" -j4 >/dev/null
        src="$AM/apps/mimo/build/mimo-$impl.bin"
        [ -f "$src" ] || { echo "ERROR: missing $src" >&2; exit 1; }
        dst="$IMG/mimo/mimo-$impl-$kernel.bin"
        cp -f "$src" "$dst"
        printf 'mimo\t%s\t%s\n' "mimo-$impl-$kernel" "$dst" >> "$OUT/images.tsv"
    done
done

echo "[bupt] done: $(grep -c '^fft' "$OUT/images.tsv") FFT + $(grep -c '^mimo' "$OUT/images.tsv") MIMO"
echo "manifest: $OUT/images.tsv"
