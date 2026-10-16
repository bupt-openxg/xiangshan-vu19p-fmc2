# PHINE PD-VU19P-2-LSI SOLO v3 板卡配置清单（一次性）

> 本文档对应 `third_party/env-scripts/fpga_diff`（由 `patches/env-scripts/` 补丁落地的移植工程）产生的比特流。
> 时钟频率与 `vivado/constr/common/fpga.xdc` 中的约束一一对应，改动时须同步。

## 1. 硬件连接

| 项目 | 板卡接口 | 说明 |
|---|---|---|
| FPGA 配置/调试 | AMD JTAG CN10（14-pin 2×7） | 烧比特流 + JTAG-to-AXI 写 DDR 必须 |
| CPU 串口 | 后面板 RJ45 用户串口（BG14=TX / BH14=RX） | 需 RJ45→USB 串口适配器，115200 8N1 |
| 板卡管理 | USB 3.0 Type-B 或后板 GbE | AEConsole 连板卡控制器（ZYNQ） |
| DDR4 子卡 | FMC2 HPC 槽（UV-FMCH-PDDR4DME + 1Rx8 8GB SODIMM） | 上电前插好 |

## 2. AEConsole 一次性配置（不可省）

### 2.1 FMC2 电压
1. 打开 AEConsole，连接板卡控制器，**上电前**设置：`FMC2 VADJ = 1.2V`（DDR4 电平）。
2. 该设置持久保存，之后上电均需 VADJ=1.2V。

### 2.2 Global Clock（PLL 全局时钟）
左侧面板 `Global Clock` → PLL Clock Configuration：

| 输入选择 | 输出通道 | 输出频率 | 去向（FPGA 引脚） | 用途 |
|---|---|---|---|---|
| OSC 54MHz | HP20_GC02 | **80 MHz** | BM44/BN44 → clk7_p/n | DDR4 MIG 参考时钟 |
| OSC 54MHz | HP26_GC04 | **25 MHz** | BA51/BB51 → clk5_p/n | CPU / 系统时钟 |
| OSC 54MHz | HP35_GC06 | **1 MHz** | E57/E58 → clk8_p/n | RTC（tmclk） |

之后点击 `Program` 写入时钟模块。参考时钟 `HP20_GC02` 是用户手册 §6.1.4 DDR4 示例的推荐通道。
> 备注：若 `1 MHz` 被拒绝（范围下限标称 1.000MHz），改配 `4 MHz` 并同步把
> fpga.xdc 中 `TMCLK` 的 `create_clock -period` 改为 250.000（RTC 精度无关紧要）。

### 2.3 供电时序
1. 确认 FMC2 VADJ=1.2V 与 PLL 配置已生效；
2. 断电重新上电（让电压/时钟配置对手动上电流程生效）；
3. 检查 DDR 子卡 LED1（init OK 状态由上板后的 FPGA 逻辑驱动，见阶段4）。

## 3. 时钟对照表（XDC ↔ AEConsole）

| XDC 端口 | 频率 | 来源 | create_clock |
|---|---|---|---|
| clk6_p/n (BW37/BY37) | 200 MHz | 板载固定 OSC（不可配置） | `CPU_CLK_IN` period 5.000 |
| clk5_p/n (BA51/BB51) | 25 MHz | PLL GC04 | `DEBUG_CLK_IN` period 40.000 |
| clk7_p/n (BM44/BN44) | 80 MHz | PLL GC02 | （MIG 内部约束, 顶层自动派生） |
| clk8_p/n (E57/E58) | 1 MHz | PLL GC06 | `TMCLK` period 1000.000 |
| JTAG_TCK (W13) | ≤12 MHz | 外部调试器 | `jtag_vclk` period 83.333 |

## 4. 复位与状态

- **PUSHSW1**（BG18）= 系统复位（低有效）；**PUSHSW2**（BG17）= CPU 复位；
- DDR 复位默认上拉释放，实际由 JTAG VIO `vio_sw4` 控制；
- LED1（AA16）= cpu_wr_ddr_valid、LED2（AA14）= cpu_rd_qspi_valid、LED3（AB13）= **init_calib_complete**（DDR 校准完成标志）;
- RISC-V 调试 JTAG 引出在 GPIO 排针 CN43（GPIO_1..5 = W13/V14/U13/Y13/Y15，LVCMOS33），
  供 J-Link/FT2232 + OpenOCD 使用（可选）。

## 5. 已知差异（相对香山官方 ai1 板流程）

- 器件: `xcvu19p-fsva3824-1-e`（BSP 内官方 DCP 标称；IDCODE 应为 0x04BA1093）
- DDR 引脚全部映射到 FMC2 槽（`pinmaps/fmc2_ddr4_pins.csv`，经 UG §6.1.2.1 与 BSP 引脚 xls 交叉验证）
- MIG 参数保持官方 ai1 配置：SODIMM / 64bit / 单 rank / 800MT/s / 80MHz 参考时钟
- 时钟源改为 PHINE PLL GC 输出（见 2.2），UART 换到 PHINE RJ45 串口，SD/网口约束已移除
