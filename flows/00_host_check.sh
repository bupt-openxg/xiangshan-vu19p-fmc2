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
for t in git curl python3 unzip sha256sum dpkg-deb tar zstd; do
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
    if [ "$mem" -ge 32 ]; then
        ok "RAM ${mem}GB"
    else
        warn "RAM ${mem}GB：本机只做 RTL 生成/软件构建/上板；建议 ≥32GB（Mill 生成 RTL）。Vivado 综合在服务器上跑（125GB）"
    fi
fi
if [ -n "${swap:-}" ]; then
    if [ "$swap" -ge 16 ]; then
        ok "swap ${swap}GB"
    else
        warn "swap ${swap}GB（RTL 生成辅助项；综合在服务器上跑，本机建议 ≥16GB）"
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
