#!/usr/bin/env bash
# =============================================================================
# 03 生成香山 RTL + 模拟器残留检查 + RTL 后处理
#   产物: third_party/XiangShan/build/rtl（V2: 1579 个文件；V3 数量在建后核对）
#   耗时: 约 30-60 分钟（mill + GraalVM 21；V3 的 mill 0.12.17 首次运行需下载依赖）
#   配置: CONFIG=MinimalConfig（V2/V3 同名；V3 内含 OpenLLC 4MB + L2 128KB + L1D 32KB；
#         若资源/时序紧张, 可加 L2_CACHE_SIZE/L3_CACHE_SIZE 缩小缓存）
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true

[ -d "$NOOP_HOME" ] || { echo "ERROR: 未找到香山源码 $NOOP_HOME"; echo "  先运行 flows/02_clone_deps.sh"; exit 1; }
mkdir -p "$BUILD_ROOT/check"

# 注意: V3 的 stock MinimalConfig 含 OpenLLC 4MB + 8-wide 前端/后端 + 向量单元,
# 综合 elaboration 工作集 ~110GB —— 需要 ≥64GB 物理内存的构建机（swap 无法替代,
# 15GB RAM 下即使 128GB swap 也只是磁盘抖动停滞）。本工程保持 stock 配置, 不做裁剪。
echo "[03] make verilog CONFIG=MinimalConfig JVM_XMX=${JVM_XMX:-20G}"
cd "$NOOP_HOME"
make verilog CONFIG=MinimalConfig JVM_XMX="${JVM_XMX:-20G}"

echo "[03] 模拟器残留检查（门控感知）"
bash "$ROOT/tools/checks/rtl_check.sh" "$NOOP_HOME/build/rtl" "$BUILD_ROOT/check/rtl-check-report.md" >/dev/null
tail -6 "$BUILD_ROOT/check/rtl-check-report.md"
echo "  报告: $BUILD_ROOT/check/rtl-check-report.md"

echo "[03] RTL 后处理（ClockGate 透传 / 大数组 URAM 提示 / 去 DummyDPI 例化链）"
bash "$ROOT/tools/patch_rtl.sh" "$NOOP_HOME/build/rtl" | tail -8

echo
echo "==== RTL 就绪: $NOOP_HOME/build/rtl（$(ls "$NOOP_HOME/build/rtl" | wc -l) 个文件）===="
echo "下一步: flows/04_build_bitstream.sh（创建工程+综合+实现+比特流，约 7 小时）"
