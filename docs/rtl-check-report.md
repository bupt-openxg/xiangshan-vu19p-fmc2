# RTL 模拟器残留检查报告（门控感知）

- 检查目录: `third_party/XiangShan/build/rtl`
- 生成时间: 2026-10-21 15:45:01
- 文件数: 1819

## 1. 顶层与模拟器专属模块
- [OK] 顶层 XSTop.sv 存在（FPGA 顶层）
- [OK] 未发现模拟器专属模块

## 2. DPI-C import（无 S YNTHESIS 保护 = 危险）
- [OK] 无无保护的 DPI import（另有 21 个文件在 `ifndef SYNTHESIS 保护内, 综合时被编译掉）

## 3. 仿真专用系统任务（$display/$readmem/plusargs 等, 无保护区）
- [WARN] 无保护区命中 `release `:
    third_party/XiangShan/build/rtl/MSHR.sv:281:     ~state_s_release & state_w_rprobeacklast & state_w_grantlast & state_w_replResp;
    third_party/XiangShan/build/rtl/MSHR.sv:307:     state_s_refill & state_s_probeack & state_s_release & state_w_rprobeacklast
    third_party/XiangShan/build/rtl/MSHR.sv:364:       state_s_release <= 1'h1;

## 4. Difftest 残留（标识符引用 = WARN; 无保护 DPI import 已在第2节按 ERROR 处理）
- [WARN] 32 个文件存在 difftest 标识符引用（例化 DummyDPICWrapper/Difftest* 空模块链）:
    third_party/XiangShan/build/rtl/AtomicsUnit.sv:191:   wire         difftest_lrsc_success = success;
    third_party/XiangShan/build/rtl/AtomicsUnit.sv:248:   wire         _difftest_valid_T_1 = state == 4'h7;
    third_party/XiangShan/build/rtl/AtomicsUnit.sv:280:   wire [7:0]   difftest_lrsc_coreid = {2'h0, io_hartId};
    third_party/XiangShan/build/rtl/AtomicsUnit.sv:281:   wire         difftest_gen_valid = io_out_valid_0 & _difftest_valid_T_1 & isSc;
    third_party/XiangShan/build/rtl/AtomicsUnit.sv:367:   wire         _GEN_15 = _difftest_valid_T_1 & io_out_valid_0;
    third_party/XiangShan/build/rtl/AtomicsUnit.sv:789:   DummyDPICWrapper_LrScEvent difftest_lrsc_1 (
    (这些模块体已被 `ifndef SYNTHESIS 门控, 综合时退化为空模块并被优化; 
     危险形式 = 无保护 import "DPI-C", 已在第2节校验为零; 最终以 Vivado 综合为仲裁)

## 5. 断言 (--remove-assert 应已清除, 无保护区)
- [OK] 无 assert

## 6. 关键端口抽查 (XSTop 顶层, 与 fpga_diff wrapper 匹配)
- [OK] port `io_clock`
- [OK] port `io_reset`
- [OK] port `io_sram_config`
- [OK] port `io_extIntrs`
- [OK] port `io_rtc_clock`
- [OK] port `io_riscv_rst_vec_0`
- [OK] port `memory_awready`
- [OK] port `peripheral_awready`
- [OK] port `io_systemjtag_jtag_TCK`
- [OK] port `nmi_0_0`

## 7. 结论
**通过**: 无阻塞级残留; 所有仿真构造均被 `ifndef SYNTHESIS/`ifdef DIFFTEST 门控,
Vivado 综合 (`SYNTHESIS 已定义) 时自动剔除。WARN 项已人工确认。
