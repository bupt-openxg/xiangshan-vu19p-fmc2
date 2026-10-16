#!/usr/bin/env bash
# =============================================================================
# RTL 后处理 (官方 FPGA 文档步骤 + ai1 流程同款 URAM 提示)
#   用法: tools/patch_rtl.sh <build/rtl 目录>
#   1) ClockGate.v 去门控时钟 -> 透传 (省 BUFG; 官方文档: "去掉代码中的门控时钟")
#   2) 最大 64bit 多千深 SRAM 数组加 (* ram_style = "ultra" *) (对应 ai1 的 array_22_ext 提示)
# =============================================================================
set -euo pipefail
RTL="${1:?用法: patch_rtl.sh <build/rtl 目录>}"

# 1) ClockGate 透传
cat > "$RTL/ClockGate.v" <<'EOF'
module ClockGate (
  input  wire TE,
  input  wire E,
  input  wire CK,
  output wire Q
);
  // FPGA 移植: 去门控时钟 (官方文档步骤) — 直接透传时钟, 避免综合出大量
  // BUFGCE / gated clock, 节省全局时钟资源。功能等价。
  assign Q = CK;
endmodule
EOF
echo "[patch] ClockGate.v -> 透传"

# 2) 大数组 URAM 提示: 找到所有 reg [63:0] ram [N:0] (N>=4095) 的 array 文件
for f in "$RTL"/array_*.v; do
    [ -f "$f" ] || continue
    if grep -q "reg \[63:0\] ram \[[0-9]\{4,\}:0\];" "$f" && ! grep -q 'ram_style = "ultra"' "$f"; then
        sed -i 's/^\(\s*\)reg \(\[63:0\] ram \[[0-9]\{4,\}:0\];\)/\1(* ram_style = "ultra" *) reg \2/' "$f"
        echo "[patch] $(basename "$f"): 加 (* ram_style = \"ultra\" *)"
    fi
done

# 3) 移除 DummyDPICWrapper 例化链 (difftest DPI 包装, 无 difftest 时是悬空链;
#    Vivado 综合会因 Difftest* 模块未解析产生 INBB-3 黑盒错误 → 整体删除)
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
echo "[patch] 完成"
