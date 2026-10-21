#!/usr/bin/env bash
# =============================================================================
# 09 构建 BUPT 完整负载矩阵（48 FFT + 12 MIMO）
#   详细说明见 tests/README.md；本脚本只是 flows 编号入口。
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
exec bash "$ROOT/tests/bupt/build.sh" "$@"
