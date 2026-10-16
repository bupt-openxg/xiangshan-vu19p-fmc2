#!/usr/bin/env bash
# 工具: 给 ROM (bootrom) 打版本兼容启动补丁 (见 tools/patch_bootrom.tcl 说明)
# 用法: bash tools/patch_bootrom.sh
# 时机: FPGA 每次重新配置后、reset_cpu 之前; 或者在串口无任何输出时排查用。
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh"
vivado -mode batch -nojournal -nolog -source "$ROOT/tools/patch_bootrom.tcl" 2>&1 | \
    grep -E "写入|BRESP|BOOTROM PATCH DONE|ERROR" | head -8
