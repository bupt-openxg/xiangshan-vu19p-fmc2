# 板卡诊断比特流（hw/diag，可用 flows/08_board_diag.sh 一键构建+运行）

## 用途

在不依赖任何 XiangShan 逻辑的前提下，用最小设计与 **VIO 输入探针**给出板卡地面真相：

- 四路时钟是否存活（clk5 25M / clk6 200M / clk7 80M / clk8 1M，各一个 32-bit 计数器）
- 三个复位引脚电平（rstn_sw6/5/4 → p6/p5/p4）
- 与 XiangShan 相同的 `cpu_rstn` 释放逻辑（消抖 FSM + VIO sw5 上升沿直通）
- **串口链路端到端**：VIO 控制一个 115200 UART 发送器（25MHz/217 分频），
  在主机侧抓 `ttyUSB*` 即可验证 FPGA 引脚→适配器→主机全链路

设计文件：
- `rtl/fpga_diag_top.sv`（顶层；含 VIO 输入/输出与 UART TX）
- `rtl/button_debounce.v`（与主设计相同的消抖 FSM）
- `constr/diag.xdc`（引脚与 `third_party/env-scripts/fpga_diff/vivado/constr/common/fpga.xdc` 一致）
- `build_diag.tcl`（建工程→综合→实现→比特流，小设计约 15-25 分钟）

## 构建

```bash
source tools/env.sh
vivado -mode batch -source hw/diag/build_diag.tcl -log build/logs/diag_build.log
# 产物: hw/diag/proj/board_diag.runs/impl_1/fpga_diag_top.bit + .ltx
```

## 上板运行

```bash
# 1) 主机串口抓包（另一个终端或后台）
stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb -crtscts -echo raw
timeout 300 cat /dev/ttyUSB0 > /tmp/serial_diag.log &

# 2) 编程 + 读探针 + 发字节（全程 TCL）
source tools/env.sh
vivado -mode tcl -source hw/diag/scripts/run_diag.tcl
```

### VIO 探针一览（2026-10-14 实测环境）

| 探针 | 方向 | 含义 | 实测值 |
|---|---|---|---|
| cnt5/cnt6/cnt7/cnt8 | in | 四路时钟计数器 | 均在计数 ✓ |
| p6/p5/p4 | in | rstn_sw6/sw5/sw4 引脚 | 1/1/1 ✓ |
| uart0_sin_IBUF | in | 适配器 TXD（空闲=1） | 1 ✓ |
| cpu_rstn / cpu_setn_rflag / vio_sw5_d | in | 复位链状态 | 序列后 cpu_rstn=1 ✓ |
| ubusy / uart0_sout_OBUF | in | UART 发送状态/引脚 | 正常 |
| vio_sw6 / vio_sw5 | out | 复位控制 | INIT=1 |
| uart_data[7:0] / uart_start | out | UART 发送（HEX 无 0x 前缀，如 `48`='H'） | — |

### 注意

- 输出探针的 `set_property OUTPUT_VALUE` 用 **HEX 字符串且不带 `0x`**（`0x48` 会报
  "value characters" 错误，`48` 正确）
- 探针名是**网络名**（`cnt5`、`cpu_rstn`…），不是 `probe_in0`；同名网络上同时存在
  输入/输出探针时，硬件管理器会以 `_1` 后缀区分
- 每次新开 Vivado 会话需先 `set_property PROBES.FILE <ltx>` + `refresh_hw_device -update_hw_probes true`
  才能列出探针
- 更新设计后（如 v3 加入 7 输入探针）重跑 `build_diag.tcl` 前无需清理工程（`-force` 重建）
