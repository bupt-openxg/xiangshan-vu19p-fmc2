# 硬件与板卡（hw/）

目标板：**PHINE PD-VU19P-2-LSI SOLO v3**（Xilinx XCVU19P-2, 封装 fsva3824，IDCODE `0x04BA1093`）。
FPGA 侧工程与引脚约束由 `patches/env-scripts/` 应用到 env-scripts 后生效；
本目录提供引脚事实表、板卡配置说明与独立诊断设计。

```
hw/pinmaps/   引脚表（board_io.csv / fmc2_ddr4_pins.csv）——XDC 生成依据, 已交叉验证
hw/diag/      板卡诊断比特流（独立小工程, 分钟级构建, 上板前置自检用）
```

## 1. 上板连接（详见 README.md）

| 项目 | 说明 |
|---|---|
| JTAG（配置/烧录） | CN10（标准 14-pin），Digilent 线；`localhost:3121/xilinx_tcf/Digilent/*` |
| UART 控制台 | 后面板 **User (RJ45)**，仅 TX/RX；**115200 8N1**；适配器 = FT232R（`/dev/ttyUSB*`，用 `udevadm info -q property -n <dev> \| grep ID_MODEL` 甄别） |
| DDR4 | FMC2 槽 + UV-FMCH-PDDR4DME 子卡 + 1Rx8 16GB SODIMM（单 rank，MT40A2G8VA-062E） |
| 时钟 | AEConsole PLL 输出（见下表）；板载 200MHz OSC |
| 按键/开关 | **流程完全不使用**——复位/启动全部经 VIO + Tcl 控制 |

## 2. AEConsole 时钟配置（一次性，详见 docs/board-setup.md）

| PLL 输出 | 引脚 | 频率 | 用途 |
|---|---|---|---|
| GC02 | BM44/BN44 | **80MHz** | DDR4 参考时钟（clk7） |
| GC04 | BA51/BB51 | **25MHz** | CPU/系统时钟（clk5，本设计主时钟）|
| GC06 | E57/E58 | **1MHz** | RTC（clk8，OpenSBI 计时基准）|
| —（板载 OSC）| BW37/BY37 | 200MHz | 系统参考（clk6）|

另需将 **VADJ 设为 1.2V**（FMC2/DDR bank 电压）。AEConsole 可从厂商 BSP
（PHINE.2024.02.P6.vu19p.tar.gz，置于 `board/`）通过 `tools/setup/04/05` 解出；
纯 JTAG 烧录流程不依赖它。

## 3. 引脚表

| 文件 | 内容 | 生成脚本 |
|---|---|---|
| `pinmaps/board_io.csv` | 板卡 IO（UART/LED/按键/JTAG/GPIO）来源与球点 | 人工整理（源自厂商 Pinlist XDC）|
| `pinmaps/fmc2_ddr4_pins.csv` | FMC2 槽 DDR4 116 个信号 ↔ FPGA 球点 | `tools/build_fmc2_pinmap.py`（UG 四表交叉 + BSP xls 合并）|

由引脚表生成 DDR 约束：`tools/gen_ddr_xdc.py hw/pinmaps/fmc2_ddr4_pins.csv <out.xdc>`。
（板卡 IO 约束 `fpga.xdc` 与 DDR 约束 `ddr.xdc` 均随补丁应用到 env-scripts 工程内。）

## 4. 诊断比特流（hw/diag）

独立最小设计（VIO 输入探针 + 四路时钟计数器 + 复刻复位链 + VIO 控制 UART 发送器），
**不加载香山核**，用于在上板前/排障时分钟级验证：四路时钟存活、三个复位引脚电平、
复位释放逻辑、串口链路端到端（VIO 发字符 → 主机收到）。

```bash
source tools/env.sh
vivado -mode batch -source hw/diag/build_diag.tcl     # → hw/diag/proj/.../*.bit (~20 分钟)
vivado -mode tcl   -source hw/diag/scripts/run_diag.tcl
```

用途与实测结论见 [docs/board-diag.md](../docs/board-diag.md)。
