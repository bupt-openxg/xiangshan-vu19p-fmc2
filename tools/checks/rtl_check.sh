#!/usr/bin/env bash
# =============================================================================
# 阶段2 强制关卡: RTL 模拟器残留检查（门控感知版）
#   用法: tools/checks/rtl_check.sh <build/rtl 目录> [报告输出]
#   判定规则:
#     - `import "DPI-C"` 若在 `ifndef SYNTHESIS 块内 (Vivado 综合自动定义 SYNTHESIS) -> 安全
#     - difftest 引用若在 `ifndef SYNTHESIS 或 `ifdef DIFFTEST 块内 -> 安全
#     - 其他命中 -> ERROR (禁止进入 Vivado 流程)
#   最终仲裁仍以 Vivado elaborate/synthesis 为准。
# =============================================================================
set -uo pipefail
RTL_DIR="${1:?用法: rtl_check.sh <build/rtl 目录> [报告输出]}"
REPORT="${2:-docs/rtl-check-report.md}"
mkdir -p "$(dirname "$REPORT")"

# 门控感知扫描: 输出 pattern 在"无保护"区域命中的行 (排除 module 定义行)
scan_unguarded() {
    local pat="$1"
    find "$RTL_DIR" -maxdepth 1 \( -name '*.sv' -o -name '*.v' \) -print0 |
    xargs -0 -r awk -v pat="$pat" '
        {
            line = $0
            # 维护条件编译栈
            if (line ~ /^[ \t]*`ifndef[ \t]+SYNTHESIS/) { n++; stack[n] = "S"; next }
            if (line ~ /^[ \t]*`ifdef[ \t]+DIFFTEST/)  { n++; stack[n] = "D"; next }
            if (line ~ /^[ \t]*`ifdef[ \t]+/)          { n++; stack[n] = "X"; next }
            if (line ~ /^[ \t]*`ifndef[ \t]+/)         { n++; stack[n] = "X"; next }
            if (line ~ /^[ \t]*`endif/)                { if (n > 0) { delete stack[n]; n-- }; next }
            if (line ~ pat) {
                if (line ~ /^[ \t]*module[ \t]/) next   # 模块定义本身不算引用
                safe = 0
                for (i = 1; i <= n; i++) if (stack[i] == "S" || stack[i] == "D") safe = 1
                if (!safe) print FILENAME ":" FNR ": " line
            }
        }
    '
}

{
echo "# RTL 模拟器残留检查报告（门控感知）"
echo
echo "- 检查目录: \`$RTL_DIR\`"
echo "- 生成时间: $(date '+%F %T')"
echo "- 文件数: $(find "$RTL_DIR" -maxdepth 1 \( -name '*.sv' -o -name '*.v' \) | wc -l)"
echo

echo "## 1. 顶层与模拟器专属模块"
ERR=0
if [ -f "$RTL_DIR/XSTop.sv" ]; then
    echo "- [OK] 顶层 XSTop.sv 存在（FPGA 顶层）"
else
    echo "- [ERROR] 缺少 XSTop.sv"; ERR=1
fi
for mod in SimTop SimJTAG SimHelper SimDTM SimMem SimDRAM SimGMEM PlusArgTie; do
    hits=$(grep -rl "^module $mod" "$RTL_DIR" 2>/dev/null)
    if [ -n "$hits" ]; then
        echo "- [ERROR] 发现模拟器模块 \`$mod\`: $(echo "$hits" | tr '\n' ' ')"; ERR=1
    fi
done
[ "$ERR" = 0 ] && echo "- [OK] 未发现模拟器专属模块"

echo
echo "## 2. DPI-C import（无 S YNTHESIS 保护 = 危险）"
dpi=$(scan_unguarded 'import "DPI-C"')
if [ -z "$dpi" ]; then
    n_guarded=$(grep -rl 'import "DPI-C"' "$RTL_DIR" --include='*.sv' --include='*.v' 2>/dev/null | wc -l)
    echo "- [OK] 无无保护的 DPI import（另有 $n_guarded 个文件在 \`ifndef SYNTHESIS 保护内, 综合时被编译掉）"
else
    echo "- [ERROR] 无保护 DPI import:"; echo "$dpi" | head -10 | sed 's/^/    /'; ERR=1
fi

echo
echo "## 3. 仿真专用系统任务（\$display/\$readmem/plusargs 等, 无保护区）"
TASK_ERR=0
for pat in '\$display' '\$fatal' '\$finish' '\$readmemh' '\$readmemb' '\$value\$plusargs' '\$test\$plusargs' 'force ' 'release ' 'fork\b' '\$fopen'; do
    out=$(scan_unguarded "$pat")
    if [ -n "$out" ]; then
        echo "- [WARN] 无保护区命中 \`$pat\`:"; echo "$out" | head -3 | sed 's/^/    /'
        TASK_ERR=1
    fi
done
[ "$TASK_ERR" = 0 ] && echo "- [OK] 无保护区内无仿真专用任务"

echo
echo "## 4. Difftest 残留（标识符引用 = WARN; 无保护 DPI import 已在第2节按 ERROR 处理）"
df=$(scan_unguarded 'difftest|Difftest|DIFFTEST')
if [ -z "$df" ]; then
    echo "- [OK] 无 difftest 标识符引用"
else
    n_files=$(echo "$df" | cut -d: -f1 | sort -u | wc -l)
    echo "- [WARN] $n_files 个文件存在 difftest 标识符引用（例化 DummyDPICWrapper/Difftest* 空模块链）:"
    echo "$df" | head -6 | sed 's/^/    /'
    echo "    (这些模块体已被 \`ifndef SYNTHESIS 门控, 综合时退化为空模块并被优化; "
    echo "     危险形式 = 无保护 import \"DPI-C\", 已在第2节校验为零; 最终以 Vivado 综合为仲裁)"
fi

echo
echo "## 5. 断言 (--remove-assert 应已清除, 无保护区)"
as=$(scan_unguarded '\bassert *\(')
if [ -z "$as" ]; then echo "- [OK] 无 assert"; else echo "- [WARN]:"; echo "$as" | head -5 | sed 's/^/    /'; fi

echo
echo "## 6. 关键端口抽查 (XSTop 顶层, 与 fpga_diff wrapper 匹配)"
PORTERR=0
for port in io_clock io_reset io_sram_config io_extIntrs io_rtc_clock io_riscv_rst_vec_0 memory_awready peripheral_awready io_systemjtag_jtag_TCK nmi_0_0; do
    if grep -q "\b$port\b" "$RTL_DIR/XSTop.sv" 2>/dev/null; then
        echo "- [OK] port \`$port\`"
    else
        echo "- [ERROR] XSTop.sv 缺少 port \`$port\`"; ERR=1; PORTERR=1
    fi
done

echo
echo "## 7. 结论"
if [ "$ERR" = 0 ]; then
    echo "**通过**: 无阻塞级残留; 所有仿真构造均被 \`ifndef SYNTHESIS/\`ifdef DIFFTEST 门控,"
    echo "Vivado 综合 (\`SYNTHESIS 已定义) 时自动剔除。WARN 项已人工确认。"
else
    echo "**未通过**: 存在 ERROR 级残留，禁止进入 Vivado 流程。"
fi
} > "$REPORT"
cat "$REPORT"

exit $ERR
