#!/usr/bin/env bash
# =============================================================================
# RTL 后处理 (官方 FPGA 文档步骤 + ai1 流程同款 URAM 提示)
#   用法: tools/patch_rtl.sh <build/rtl 目录>
#   1) ClockGate 去门控时钟 -> 透传 (省 BUFG; 官方文档: "去掉代码中的门控时钟")
#      V2 生成 ClockGate.v；V3 (--fpga-platform) 生成 ClockGate.sv —— 两代都处理
#   2) [仅 V2] 最大 64bit 多千深 SRAM 数组加 (* ram_style = "ultra" *)
#      V2 的数组在 array_*.v 中；V3 内存为 SRAM 模板, 无 array_*.v -> 本步自然跳过
#   3) DummyDPICWrapper 例化链:
#      V2 只生成例化、不生成模块定义 -> Vivado 报 INBB-3 黑盒错误 -> 删除例化;
#      V3 同时生成模块定义且 DPI import 由 ifndef SYNTHESIS 门控 -> 保留例化
# =============================================================================
set -euo pipefail
RTL="${1:?用法: patch_rtl.sh <build/rtl 目录>}"

# 1) ClockGate 透传（保持端口名与生成物一致: TE/E/CK/Q）
if [ -f "$RTL/ClockGate.sv" ]; then
    GATE="$RTL/ClockGate.sv"
elif [ -f "$RTL/ClockGate.v" ]; then
    GATE="$RTL/ClockGate.v"
else
    GATE="$RTL/ClockGate.v"
fi
cat > "$GATE" <<'EOF'
// FPGA 移植: 去门控时钟 (官方文档步骤) — 直接透传时钟, 避免综合出大量
// BUFGCE / gated clock, 节省全局时钟资源。功能等价。
module ClockGate (
  input  wire TE,
  input  wire E,
  input  wire CK,
  output wire Q
);
  assign Q = CK;
endmodule
EOF
echo "[patch] $(basename "$GATE") -> 透传"

# 2) 大数组 URAM 提示: 找到所有 reg [63:0] ram [N:0] (N>=4095) 的 array 文件（仅 V2）
for f in "$RTL"/array_*.v; do
    [ -f "$f" ] || continue
    if grep -q "reg \[63:0\] ram \[[0-9]\{4,\}:0\];" "$f" && ! grep -q 'ram_style = "ultra"' "$f"; then
        sed -i 's/^\(\s*\)reg \(\[63:0\] ram \[[0-9]\{4,\}:0\];\)/\1(* ram_style = "ultra" *) reg \2/' "$f"
        echo "[patch] $(basename "$f"): 加 (* ram_style = \"ultra\" *)"
    fi
done

# 3) DummyDPICWrapper 例化: 仅当缺少模块定义时才删除（V2 情形）
if compgen -G "$RTL/DummyDPICWrapper*.sv" >/dev/null || compgen -G "$RTL/DummyDPICWrapper*.v" >/dev/null; then
    echo "[patch] DummyDPICWrapper 模块定义存在（V3 形态, DPI 已由 SYNTHESIS 门控）-> 保留例化"
else
    python3 - "$RTL" <<'PYEOF'
import re, sys, glob, os
rtl = sys.argv[1]
pat = re.compile(r"^\s*(DummyDPIC\w*)\s+(\w+)\s*\(")
total = 0
for f in sorted(glob.glob(os.path.join(rtl, "*.sv")) + glob.glob(os.path.join(rtl, "*.v"))):
    lines = open(f).readlines()
    out, i, removed = [], 0, 0
    while i < len(lines):
        m = pat.match(lines[i])
        if m:
            depth, started = 0, False
            while i < len(lines):
                for ch in lines[i]:
                    if ch == '(':
                        depth += 1; started = True
                    elif ch == ')':
                        depth -= 1
                i += 1
                if started and depth <= 0:
                    break
            removed += 1
            continue
        out.append(lines[i]); i += 1
    if removed:
        open(f, "w").writelines(out)
        print(f"[patch] {os.path.basename(f)}: 移除 {removed} 处 DummyDPICWrapper 例化")
        total += removed
print(f"[patch] DummyDPICWrapper 例化共移除 {total} 处")
PYEOF
fi
echo "[patch] 完成"
