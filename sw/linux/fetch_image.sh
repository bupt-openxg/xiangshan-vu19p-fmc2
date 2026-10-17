#!/usr/bin/env bash
# =============================================================================
# 下载官方昆明湖 FPGA 内存镜像 (data_kmh.zip) 并解压为 data.txt
#
#   官方出处: XiangShan-doc 仓库 docs/integration/fpga/fpga.md
#             （"实例昆明湖内存镜像txt" 一行, 渲染页 https://docs.xiangshan.cc/zh-cn/dev/integration/fpga/）
#   固定校验: 5,178,920 字节 / sha256 如下（与官方 git blob 一致）
#   可用 KMH_IMAGE_URL 环境变量覆盖下载地址（如镜像站点）
#
#   注意: 本设计 UART 时钟为 25MHz，而镜像内嵌 DTB 假定 50MHz，
#         启动前需经 tools/patch_linux_dtb_clock.py 生成修正版
#         （tools/run_linux.sh 会自动处理）。
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
DEST="$ROOT/sw/linux"
URL="${KMH_IMAGE_URL:-https://raw.githubusercontent.com/OpenXiangShan/XiangShan-doc/main/docs/integration/resources/data_kmh.zip}"
SHA256="483e077e0ef8d4e8afc8c896273fa239f5f871ed5b38459de3fc7cab3671876c"
ZIP="$DEST/data_kmh.zip"

verify() { echo "$SHA256  $1" | sha256sum -c --status - 2>/dev/null; }

mkdir -p "$DEST"
ok=0
if [ -f "$ZIP" ] && verify "$ZIP"; then
    ok=1
    echo "[fetch] 已存在且校验通过，跳过下载"
fi

if [ "$ok" = 0 ]; then
    echo "[fetch] 下载官方镜像: $URL"
    for i in $(seq 1 8); do
        # --http1.1           : raw.githubusercontent.com 上 HTTP/2 偶发 PROTOCOL_ERROR，降级规避
        # -C -                : 断点续传（.part 跨重试复用）
        # --speed-time/-limit : 低于 1KB/s 持续 60s 视为卡死，主动断开进入下一轮续传
        if curl -fL --http1.1 -C - \
                --retry 3 --retry-delay 3 --retry-all-errors \
                --connect-timeout 20 --speed-time 60 --speed-limit 1024 \
                -o "$ZIP.part" "$URL"; then
            if verify "$ZIP.part"; then
                mv -f "$ZIP.part" "$ZIP"; ok=1
                echo "[fetch] 下载完成（第 $i 轮）"
                break
            fi
            echo "[fetch] 第 $i 轮: 校验不符，清空重下"
            rm -f "$ZIP.part"
        else
            echo "[fetch] 第 $i 轮: 连接中断，保留 .part 续传"
        fi
        sleep 3
    done
    [ "$ok" = 1 ] || { echo "[fetch] ERROR: 多次尝试后仍失败（可设 KMH_IMAGE_URL 指向镜像站点）"; exit 1; }
fi

echo "[fetch] 校验 sha256 ..."
echo "$SHA256  $ZIP" | sha256sum -c - || {
    echo "[fetch] ERROR: sha256 校验失败（文件损坏或来源变更）"
    echo "        如为上游变更, 请核对 https://github.com/OpenXiangShan/XiangShan-doc/tree/main/docs/integration/resources"
    exit 1
}

if [ -f "$DEST/data.txt" ]; then
    echo "[fetch] data.txt 已存在, 跳过解压"
else
    unzip -o "$ZIP" -d "$DEST" >/dev/null
fi
echo "[fetch] OK: $DEST/data.txt ($(stat -c %s "$DEST/data.txt") 字节)"
