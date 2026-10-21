#!/usr/bin/env bash
# =============================================================================
# 04 打包综合所需输入（生成 RTL 与 env-scripts 归档）
#   产物: build/bupt-server-<date>/{rtl.tar.zst,env-scripts-src.tar.zst,
#                                 build_server.tcl,SHA256SUMS}
#   综合机只需 RTL + env-scripts + build_server.tcl
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true

RTL="$NOOP_HOME/build/rtl"
ES="$FPGA_ROOT/.."
OUT="${BUILD_ROOT}/bupt-server-$(date +%Y%m%d)"
[ -d "$RTL" ] || { echo "ERROR: 未找到 $RTL；先跑 flows/03_gen_rtl.sh" >&2; exit 1; }

command -v zstd >/dev/null || { echo "ERROR: 需要 zstd" >&2; exit 1; }
mkdir -p "$OUT"

# DifftestMacros.svh 不在 cpu_files.tcl 的 include_dirs 里，必须随 RTL 一起放。
if [ -f "$NOOP_HOME/build/generated-src/DifftestMacros.svh" ]; then
    cp -f "$NOOP_HOME/build/generated-src/DifftestMacros.svh" "$RTL/DifftestMacros.svh"
fi

echo "[04] 打包 RTL（去掉 FIRRTL/CSV/anno 中间件）"
tar --exclude='*.fir' --exclude='*.csv' --exclude='*.anno.json' \
    -C "$NOOP_HOME/build" -cf - rtl \
  | zstd -f -T0 -19 -q -o "$OUT/rtl.tar.zst"

echo "[04] 打包 env-scripts（去掉 Vivado 工程/日志）"
tar --exclude='.git' --exclude='fpga_diff/fpga_vivado*' --exclude='fpga_diff/.Xil' \
    --exclude='fpga_diff/*.log' --exclude='fpga_diff/*.jou' \
    -C "$ES" -cf - . \
  | zstd -f -T0 -19 -q -o "$OUT/env-scripts-src.tar.zst"

cp -f "$ROOT/tools/build_server.tcl" "$OUT/build_server.tcl"
(cd "$OUT" && sha256sum rtl.tar.zst env-scripts-src.tar.zst build_server.tcl > SHA256SUMS)

cat <<EOF

==== 综合输入就绪 ====
$(du -h "$OUT"/* | sed 's/^/  /')

复制到综合机（按实际环境替换）:
  BUILD_HOST=user@synthesis-host
  BUILD_DIR=/path/to/xvbbp-build
  ssh "\$BUILD_HOST" "mkdir -p '\$BUILD_DIR'"
  scp "$OUT"/* "\$BUILD_HOST:\$BUILD_DIR/"

综合机上执行:
  cd "\$BUILD_DIR"
  sha256sum -c SHA256SUMS
  mkdir -p rtl env-scripts out
  tar --use-compress-program='zstd -d' -xf rtl.tar.zst -C rtl
  tar --use-compress-program='zstd -d' -xf env-scripts-src.tar.zst \\
      -C env-scripts --strip-components=1
  source /path/to/Vivado/2024.2/settings64.sh
  cd env-scripts/fpga_diff
  make project CPU=kmh NO_DIFF=1 DDR_RANK_WIDTH=1 CORE_DIR="\$BUILD_DIR/rtl"
  vivado -mode batch -source ../../build_server.tcl -tclargs \\
      "\$PWD/fpga_vivado_kmh-nodiff/fpga_vivado_kmh-nodiff.xpr" \\
      "\$BUILD_DIR/out" 16
EOF
