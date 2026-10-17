---
name: xiangshan-vu19p-board-ops
description: 在 PHINE PD-VU19P 板卡上运行与排障时使用：烧录、裸机 Hello、官方 Linux 镜像、串口/波特率、纯 VIO/TCL 控制与复位序列、Vivado/Labtools 报错（44-152、27-2083、44-49）对照。触发词：上板、烧录、串口无输出、VIO、hello、linux、JTAG、复位、115200、probe。
---

# 板卡操作（只用 VIO/TCL，不碰板上按键）

## 控制模型

板上按键/拨码不可用（无上下拉，且本设计把它们路由成 VIO 探针）；一切经 JTAG + VIO：

| 动作 | 命令（在 `third_party/env-scripts/fpga_diff` 下）|
|---|---|
| 暂停 CPU | `make halt_soc FPGA_BIT_HOME=<bit目录>`（sw6=0）|
| 写 DDR 负载 | `make write_ddr WORKLOAD=<xxx.txt>`（JTAG-AXI 直写内存）|
| 复位并释放 CPU | `make reset_cpu FPGA_BIT_HOME=<bit目录>` |
| 复位 DDR | `make reset_ddr FPGA_BIT_HOME=<bit目录>` |

复位序列语义（补丁 0010-0012）：`sw6=1 → sw5=0 → sw5=1`（sw5 上升沿释放 `cpu_rstn`）。
一键入口：`flows/05`（烧录）→ `flows/06`（Hello）/ `flows/07`（Linux）；底层工具 `tools/run_hello.sh`、`tools/run_linux.sh`、`tools/prog_bitstream.tcl`、`tools/read_probes.tcl`（dump 全部 VIO 探针）。

## Vivado/Labtools 报错对照（全部已修复，改脚本时不要回退）

- **44-152**（`hw_probe '<const0>' does not have property OUTPUT_VALUE`）：某探针被 `set_property` 修改后，`get_hw_probes <pattern>` 会回退为返回 VIO 的**全部**探针（含常量探针）。修复法（补丁 0012 的 `vio_probe_out` proc）：按名字精确解析输出探针 + 检查 `OUTPUT_VALUE` 能力，解析一次后**复用同一对象**，不要反复用模式查询。
- **27-2083**（Data value longer than LEN=128/SIZE=32）：写 DDR 需选 64 位 DDR 的 jtag-axi 核（补丁 0008 自动选择，勿手工改回）。
- **44-49**（frequency 12000000 not valid）：不要给 hw_target 设 frequency（补丁 0006/0007 已彻底移除）。
- JTAG 同一时刻只允许一个 Vivado 会话；TCK 已从 HDGC 挪到普通 IO（Y15，补丁 0005）；下载线接 CN10。

## 串口与时钟一致性（v4 关键修复，改一处必查另两处）

- 主机侧一律 **115200 8N1**；链路实际 ≈111.6kbps（25MHz/16/14），两端都对 3% 误差容忍。
- **UART 分频必须由程序自设**：16550 复位默认分频 162 = **9600 波特**（上板实测读 DLL=0xA2 → 串口全静默）。裸机与 OpenSBI 均写 `LCR=0x83 → DLL=14, DLM=0 → LCR=0x03`。
- **Linux 必须用时钟修正版镜像**：官方 `data_kmh.zip` 内嵌 DTB 写 UART clock=50MHz，与本设计 25MHz 不符 → 内核静默（只有 OpenSBI 横幅）。`tools/patch_linux_dtb_clock.py` 把 clock-frequency 改为 25,000,000；`flows/07` 自动生成 `sw/linux/data_patched.txt`。官方直链与 sha256 见 `sw/linux/README.md`。
- 串口设备甄别（FT232R）：`udevadm info -q property -n /dev/ttyUSBx | grep ID_MODEL`；抓包：`stty -F <dev> 115200 cs8 -cstopb -parenb -crtscts -echo raw && cat <dev>`。
- **`stty` 必须显式 `-echo`（实测踩坑）**：GNU `stty raw` **不清 echo**（echoe/echok 也保留）。echo 开着时主机行规程会把 FPGA 发来的每个字节**回写给 FPGA**（自环回）→ 客机收到自己的输出：内核日志出现 `ttyS0: N input overrun(s)`、init/console 反复被打断、shell 无法交互。典型鉴别：裸机 Hello 正常（只发不收），但 Linux 启动日志重复交织、键入命令无响应。修复/自检：`stty -F $SER ... -echo raw`，再 `stty -F $SER -a | grep echo` 确认 `-echo`。

## 预期输出

- **Hello**：`Hello, XiangShan! (MinimalConfig on PHINE PD-VU19P, 115200 8N1)`。
- **Linux**：OpenSBI v1.5.1 横幅 → Linux 6.10.7 内核日志 → buildroot `#` 提示符（`uname -a`、`ls /` 可用）；13.3MB 镜像写入约 55 秒，reset 后 ~4 秒进 shell。Hello 与 Linux 可反复切换，无需重烧比特流。

## 证据与深挖

- 串口实录：`docs/evidence/`（含 Hello 成功、Linux 完整启动、DTB 损坏取证、DDR 多读比对）。
- 时序与坑位细节：`docs/build-log.md`；其中"JTAG 读到的 DDR 与 CPU 缓存视图不一致"是**红鲱鱼**——CPU/内核读到的数据始终正确，比较 CPU 写入的内存务必以 CPU 侧视图为准。
- 板卡外部条件（四路时钟/复位电平/串口回环）用 `flows/08_board_diag.sh` 先排除（~20 分钟小工程）。
