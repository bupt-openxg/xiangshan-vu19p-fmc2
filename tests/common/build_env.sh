#!/usr/bin/env bash
# 测试构建公共环境（被各 run 脚本 source）
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true

AM="$ROOT/third_party/nexus-am"
CROSS="${RISCV_XPACK_PREFIX:?missing xPack RISC-V toolchain}"
TESTS="$ROOT/tests"
OUT="${TESTS_BUILD:-$ROOT/build/tests}"

COMMON=(
    ARCH=riscv64-xs
    AM_HOME="$AM"
    CROSS_COMPILE="$CROSS"
    RVV_TOOLCHAIN_PREFIX="$CROSS"
    "CC_OPT=-O2 -Wno-error=unused-variable -Wno-error=unused-function"
)

build_ext_test() { # $1 = output bin
    local out=$1
    mkdir -p "$(dirname "$out")"
    make -C "$TESTS/ext-instruction-test/app" \
        "${COMMON[@]}" \
        CPPFLAGS=-DUART16550=1 \
        DST_DIR="$OUT/ext-instruction-test/objects/" \
        BINARY="${out%.bin}" -j1
}

build_fft_case() { # impl points idft validate repeat warmup corpus out_prefix
    local impl=$1 points=$2 idft=$3 validate=$4 repeat=$5 warmup=$6 corpus=$7 prefix=$8
    local case_dir
    case_dir="$(dirname "$prefix")"
    rm -rf "$case_dir/objects" "$case_dir/corpus" "$case_dir/overlay"
    mkdir -p "$case_dir/objects"
    python3 "$TESTS/fft/app/scripts/generate_fft_corpus.py" \
        --corpus "$corpus" --points "$points" \
        --output "$case_dir/corpus" --overlay "$case_dir/overlay" \
        > "$case_dir/corpus.json"
    local hpm=0
    make -C "$TESTS/fft/app" -o am -o klib -o prompt \
        "${COMMON[@]}" \
        DFT_IMPL="$impl" FFT_EVIDENCE_ONLY=1 \
        FFT_POINTS="$points" FFT_REPEAT="$repeat" FFT_WARMUP_REPEAT="$warmup" \
        FFT_RUN_IDFT="$idft" FFT_VALIDATE="$validate" \
        FFT_ROI_HPM_ENABLE="$hpm" \
        CPPFLAGS="-I$case_dir/overlay -DUART16550=1" \
        DST_DIR="$case_dir/objects/" BINARY="$prefix" -j1
}

build_mimo_case() { # impl kernel perf out_bin [extra make args...]
    local impl=$1 kernel=$2 perf=$3 out=$4
    shift 4
    local case_dir
    case_dir="$(dirname "$out")"
    mkdir -p "$case_dir/objects"
    make -C "$TESTS/mimo/app" \
        "${COMMON[@]}" \
        CPPFLAGS=-DUART16550=1 \
        MIMO_IMPL="$impl" MIMO_PERF="$perf" \
        MIMO_VALIDATE_KERNEL="$kernel" MIMO_PERF_KERNEL="$kernel" \
        DST_DIR="$case_dir/objects/" BINARY="${out%.bin}" \
        "$@" -j4
}
