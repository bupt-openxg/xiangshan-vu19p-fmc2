#!/usr/bin/env bash
# 03 swap 检查（辅助项: V3 构建的真正瓶颈是物理内存）
#   V3 实测: elaboration 工作集 ~110GB+; 15GB RAM + 63GB swap -> OOM, + 128GB swap -> 磁盘抖动停滞。
#   构建机要求 RAM ≥64GB（推荐 128GB）; swap ≥64GB 作为辅助。
set -euo pipefail
SWAP_MB=$(free -m | awk '/Swap:/ {print $2}')
echo "[03] 当前 swap: $((SWAP_MB/1024)) GB"
if [ "$SWAP_MB" -lt 65536 ]; then
    echo "[03] 警告: swap < 64GB, V3 综合存在 OOM 风险（但更关键的是 RAM ≥64GB）"
    echo "     手动参考(追加 64G, 无需先 swapoff):"
    echo "       sudo fallocate -l 64G /swap-extra.img && sudo chmod 600 /swap-extra.img"
    echo "       sudo mkswap /swap-extra.img && sudo swapon /swap-extra.img"
    echo "     或单文件 128GB: sudo swapoff /swap.img && sudo rm /swap.img && \\"
    echo "       sudo fallocate -l 128G /swap.img && sudo chmod 600 /swap.img && sudo mkswap /swap.img && sudo swapon /swap.img"
elif [ "$SWAP_MB" -lt 131072 ]; then
    echo "[03] OK（swap 为辅助项; 构建机 RAM 建议 ≥64GB）"
else
    echo "[03] OK"
fi
