#!/usr/bin/env bash
# =============================================================================
# 04 打包服务器综合所需输入（本机不做综合）
#   产物: build/bupt-server-<date>/{rtl.tar.zst,env-scripts-src.tar.zst,
#                                 build_server.tcl,SHA256SUMS}
#   服务器目录: ~/.zhaoleo/xvbbp-buptopenxg（只放 RTL + env-scripts + 脚本）
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

==== 服务器输入就绪 ====
$(du -h "$OUT"/* | sed 's/^/  /')

上传:
  scp -P 22103 "$OUT"/{rtl.tar.zst,env-scripts-src.tar.zst,build_server.tcl,SHA256SUMS} \\
      turing@10.156.64.60:~/.zhaoleo/xvbbp-buptopenxg/

服务器:
  cd ~/.zhaoleo/xvbbp-buptopenxg
  sha256sum -c SHA256SUMS
  tar --use-compress-program='zstd -d' -xf rtl.tar.zst -C rtl
  tar --use-compress-program='zstd -d' -xf env-scripts-src.tar.zst -C env-scripts --strip-components=1
  source ~/zhaoleo/Xilinx/Vivado/2024.2/settings64.sh
  cd env-scripts/fpga_diff
  make project CPU=kmh NO_DIFF=1 DDR_RANK_WIDTH=1 CORE_DIR="\$HOME/.zhaoleo/xvbbp-buptopenxg/rtl"
  vivado -mode batch -source ../../build_server.tcl -tclargs \\
      "\$PWD/fpga_vivado_kmh-nodiff/fpga_vivado_kmh-nodiff.xpr" \\
      "\$HOME/.zhaoleo/xvbbp-buptopenxg/out" 16
EOF
