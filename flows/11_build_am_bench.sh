#!/usr/bin/env bash
# =============================================================================
# 11 构建 NEMU/香山常用的 AM 复杂测试（Dhrystone / CoreMark / MicroBench）
#   产物: build/am-bench/{dhrystone,coremark-2iter,microbench-test}.bin
#   运行: flows/10_run_xvbbp.sh build/am-bench/dhrystone.bin
#         flows/10_run_xvbbp.sh build/am-bench/coremark-2iter.bin
#         flows/10_run_xvbbp.sh build/am-bench/microbench-test.bin
#   说明: 预编译的 ready-to-run/*.bin 默认走 uartlite(0x40600000)，本板不可用；
#         这里用 nexus-am 源码按本板 16550 重新构建。
#   环境变量: RUN=1 构建后依次上板运行（VIO/UART 控制，见 flows/10）
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true

AM="$ROOT/third_party/nexus-am"
OUT="$BUILD_ROOT/am-bench"
CROSS="${RISCV_XPACK_PREFIX:-}"

[ -d "$AM/.git" ] || { echo "ERROR: 缺少 $AM（先运行 flows/02_clone_deps.sh）"; exit 1; }
[ -n "$CROSS" ] && [ -x "${CROSS}gcc" ] || {
    echo "ERROR: 未找到 xPack GCC 15（tools/setup/08-xpack-riscv-gcc.sh）"; exit 1;
}

COMMON=(
    ARCH=riscv64-xs
    AM_HOME="$AM"
    CROSS_COMPILE="$CROSS"
    CPPFLAGS=-DUART16550=1
    "CC_OPT=-O2 -Wno-error=unused-variable -Wno-error=unused-function"
)

# UART16550 宏不在 AM 依赖跟踪里：统一清一次库，保证串口后端正确
rm -rf "$AM/am/build"
mkdir -p "$OUT"

build_one() { # app extra-make-args... -> build/<name>.bin
    local app=$1; shift
    echo "[11] build $app $*"
    make -C "$AM/apps/$app" "${COMMON[@]}" "$@"
}

build_one dhrystone
build_one coremark ITERATIONS=2
# microbench 默认 mainargs 为空 → ref 规模，FPGA 上过慢；用 test 规模
build_one microbench mainargs=test

cp -f "$AM/apps/dhrystone/build/dhrystone-riscv64-xs.bin"                 "$OUT/dhrystone.bin"
cp -f "$AM/apps/coremark/build/coremark-2-iteration-riscv64-xs.bin"      "$OUT/coremark-2iter.bin"
cp -f "$AM/apps/microbench/build/microbench-riscv64-xs.bin"              "$OUT/microbench-test.bin"

echo
echo "==== AM 复杂测试就绪（$OUT）===="
ls -l "$OUT"
echo "运行: flows/10_run_xvbbp.sh build/am-bench/dhrystone.bin"
echo "      flows/10_run_xvbbp.sh build/am-bench/coremark-2iter.bin"
echo "      flows/10_run_xvbbp.sh build/am-bench/microbench-test.bin"

if [ "${RUN:-0}" = 1 ]; then
    CAPSEC=90  bash "$ROOT/flows/10_run_xvbbp.sh" "$OUT/dhrystone.bin"
    CAPSEC=120 bash "$ROOT/flows/10_run_xvbbp.sh" "$OUT/coremark-2iter.bin"
    CAPSEC=240 bash "$ROOT/flows/10_run_xvbbp.sh" "$OUT/microbench-test.bin"
fi
