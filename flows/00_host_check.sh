#!/usr/bin/env bash
# =============================================================================
# 00 宿主机前置检查（只读检查, 不修改任何东西）
#   - 基础工具 / 磁盘 / swap / Vivado 2024.2
#   - JTAG 与串口为软检查（板卡未连接不影响构建流程）
# =============================================================================
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
FAIL=0; WARN=0
ok()   { printf '[OK]    %s\n' "$1"; }
warn() { printf '[WARN]  %s\n' "$1"; WARN=$((WARN+1)); }
bad()  { printf '[FAIL]  %s\n' "$1"; FAIL=$((FAIL+1)); }

echo "==== ReConstruct 宿主机检查 ===="
for t in git curl python3 unzip sha256sum dpkg-deb tar; do
    command -v "$t" >/dev/null 2>&1 && ok "$t" || bad "缺少命令: $t"
done

avail=$(df -BG --output=avail "$ROOT" 2>/dev/null | tail -1 | tr -dc '0-9')
if [ -n "${avail:-}" ]; then
    [ "$avail" -ge 80 ] && ok "磁盘可用 ${avail}GB" \
        || warn "磁盘可用仅 ${avail}GB（建议 ≥80GB：Vivado 工程约 12GB + 依赖约 2GB）"
fi

mem=$(free -g 2>/dev/null | awk '/Mem:/{print $2}')
swap=$(free -g 2>/dev/null | awk '/Swap/{print $2}')
if [ -n "${mem:-}" ]; then
    if [ "$mem" -ge 64 ]; then
        ok "RAM ${mem}GB"
    else
        warn "RAM ${mem}GB：V3 stock MinimalConfig 的综合 elaboration 工作集 ~110GB+，建议构建机 RAM ≥64GB（推荐 128GB）；15GB 机器实测即使 128GB swap 也只是磁盘抖动停滞"
    fi
fi
if [ -n "${swap:-}" ]; then
    if [ "$swap" -ge 64 ]; then
        ok "swap ${swap}GB"
    else
        warn "swap ${swap}GB（V3 构建辅助项，建议 ≥64GB；swap 不能替代物理内存，见 tools/setup/03-swap.sh）"
    fi
fi

source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true
if command -v vivado >/dev/null 2>&1; then
    ok "vivado: $(vivado -version 2>/dev/null | head -1)"
else
    bad "未找到 vivado（需 Vivado 2024.2；可用 XILINX_VIVADO=/path/to/Vivado/2024.2 指定）"
fi

if ls /dev/ttyUSB* >/dev/null 2>&1; then
    ok "串口设备: $(ls /dev/ttyUSB* | tr '\n' ' ')"
else
    warn "未发现 /dev/ttyUSB*（板卡未连接时可忽略；串口适配器为 FT232R）"
fi

echo
if [ "$FAIL" = 0 ]; then
    echo "==== 通过（WARN ${WARN} 项）===="
    echo "下一步: flows/01_setup_toolchain.sh"
else
    echo "==== 存在 ${FAIL} 项 FAIL，请先修复 ===="
    exit 1
fi
