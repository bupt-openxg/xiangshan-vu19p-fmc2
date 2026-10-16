#!/usr/bin/env bash
# 03 swap 检查: 综合大设计防 OOM (本机已配置 63GB swap, 无 sudo 时仅校验)
set -euo pipefail
SWAP_MB=$(free -m | awk '/Swap:/ {print $2}')
echo "[03] 当前 swap: ${SWAP_MB} MB"
if [ "$SWAP_MB" -lt 32768 ]; then
    echo "[03] 警告: swap < 32GB, Vivado 综合存在 OOM 风险"
    echo "     手动参考: sudo fallocate -l 64G /swapfile && sudo mkswap /swapfile && sudo swapon /swapfile"
else
    echo "[03] OK"
fi
