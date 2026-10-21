#!/usr/bin/env bash
# =============================================================================
# 10 在板卡上运行 BUPT 完整负载矩阵（48 FFT + 12 MIMO）
#   前置: 已烧录比特流；FPGA_BIT_HOME 指向含 fpga_top_debug.ltx 的目录。
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
exec bash "$ROOT/tests/bupt/run.sh" "$@"
