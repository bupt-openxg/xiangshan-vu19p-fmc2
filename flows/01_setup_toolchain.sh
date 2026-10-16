#!/usr/bin/env bash
# =============================================================================
# 01 安装本地工具链（幂等；全部装进本仓库 tools/，不污染系统、不需要 sudo）
#   JDK21(GraalVM) / mill / make / riscv64 交叉工具链 / bin2fpgadata / xlrd
#   （AEConsole 与厂商 BSP 为可选项：仅时钟配置与引脚溯源需要）
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

for s in "$ROOT"/tools/setup/0*.sh; do
    echo "==== $(basename "$s") ===="
    bash "$s"
done

echo
echo "==== 工具链就绪 ===="
ls "$ROOT/tools" | tr '\n' ' '; echo
echo "下一步: flows/02_clone_deps.sh"
