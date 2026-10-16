#!/usr/bin/env bash
# =============================================================================
# ReConstruct 环境统一入口
#   用法: source tools/env.sh
# 提供: JAVA_HOME(JDK21) / mill / make(本地) / riscv64 交叉工具链 / Vivado 2024.2
#       NOOP_HOME(香山源码) / FPGA_ROOT(移植后的 fpga_diff) / BUILD_ROOT(构建产物)
# =============================================================================
_XS_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
export XS_ROOT="$_XS_ROOT"
export TOOLS_ROOT="$XS_ROOT/tools"

# ---- JDK 21 (GraalVM CE, 由 flows/01_setup_toolchain.sh 装到 tools/jdk/) ----
_JDK_DIR="$(ls -d "$TOOLS_ROOT"/jdk/graalvm-community-openjdk-21* 2>/dev/null | head -1)"
if [ -n "$_JDK_DIR" ]; then
    export JAVA_HOME="$_JDK_DIR"
    export PATH="$JAVA_HOME/bin:$PATH"
fi

# ---- 本地工具: make / riscv64 交叉工具链 (由 setup 解包到 tools/sysroot) ----
export PATH="$TOOLS_ROOT/bin:$PATH"
export PATH="$TOOLS_ROOT/sysroot/usr/bin:$PATH"
export LD_LIBRARY_PATH="$TOOLS_ROOT/sysroot/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

# ---- mill 包装脚本 (tools/mill/mill) ----
export PATH="$TOOLS_ROOT/mill:$PATH"

# ---- 第三方源码与移植代码 (由 flows/02_clone_deps.sh 拉取到 third_party/) ----
export NOOP_HOME="$XS_ROOT/third_party/XiangShan"               # 香山源码 (make verilog)
export FPGA_ROOT="$XS_ROOT/third_party/env-scripts/fpga_diff"   # 移植后的 FPGA 工程

# ---- 构建产物目录 (bitstream / 日志 / 报告) ----
export BUILD_ROOT="$XS_ROOT/build"

# ---- Vivado 2024.2 (安装路径可用 XILINX_VIVADO 覆盖) ----
_XILINX_VIVADO="${XILINX_VIVADO:-$HOME/Xilinx/Vivado/2024.2}"
if [ -f "$_XILINX_VIVADO/settings64.sh" ]; then
    source "$_XILINX_VIVADO/settings64.sh" >/dev/null 2>&1
else
    echo "[env] 警告: 未找到 $_XILINX_VIVADO/settings64.sh (Vivado 2024.2 为必需)" >&2
fi
# 许可: 优先使用已设置的 XILINXD_LICENSE_FILE, 否则探测常见位置
if [ -z "${XILINXD_LICENSE_FILE:-}" ]; then
    for _lic in "$HOME/.Xilinx/Xilinx.lic" "$HOME/下载/xilinx_ise_vivado.lic"; do
        if [ -f "$_lic" ]; then export XILINXD_LICENSE_FILE="$_lic"; break; fi
    done
fi

unset _XS_ROOT _JDK_DIR _XILINX_VIVADO _lic
