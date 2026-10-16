#!/usr/bin/env bash
# =============================================================================
# 07 运行官方 Linux 内存镜像（全 TCL/VIO 控制, 不使用板卡任何按键）
#   自动: 下载官方镜像(校验 sha256) → 生成 25MHz 时钟修正版 → halt → 写入 13.3MB
#         → 串口抓包 → reset_cpu → 约 4 秒后进入 buildroot shell
#   用法: flows/07_run_linux.sh [串口设备]     默认 /dev/ttyUSB0
#   详情: sw/linux/README.md（镜像来源/校验/时钟修正原理/预期输出）
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true

exec bash "$ROOT/tools/run_linux.sh" "$@"
