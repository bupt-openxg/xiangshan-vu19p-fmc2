---
name: xiangshan-vu19p-board-ops
description: 在 PHINE PD-VU19P 板卡上烧录、跑裸机镜像和排障时使用：VIO/TCL 复位、JTAG-AXI 写 DDR、串口 115200、Vivado/Labtools 报错（44-152、27-2083、44-49）对照。触发词：上板、烧录、串口无输出、VIO、JTAG、复位、115200、probe。
---

# 板卡操作（只用 VIO/TCL，不碰板上按键）

## 控制模型

板上按键/拨码不可用；一切经 JTAG + VIO：

| 动作 | 命令（`third_party/env-scripts/fpga_diff` 下）|
|---|---|
| 暂停 CPU | `make halt_soc FPGA_BIT_HOME=<bit目录>`（sw6=0）|
| 写 DDR | `make write_ddr WORKLOAD=<xxx.txt>`（64 位 JTAG-AXI）|
| 复位并释放 CPU | `make reset_cpu FPGA_BIT_HOME=<bit目录>` |
| 复位 DDR | `make reset_ddr FPGA_BIT_HOME=<bit目录>` |

复位序列：`sw4=1 → sw5=0 → sw6=0 → sw6=1 → sw5=1`。
BUPT 测试用 `tests/common/board_run.sh` 在一次 Vivado 会话里合并
`halt → JTAG 写 DDR → 复位 → 串口抓取`。

## 报错对照

- **44-152**：`get_hw_probes <pattern>` 可能回退返回全部探针（含 `<const0>`）。
  用 `vio_probe_out` 按精确名解析且检查 `OUTPUT_VALUE`，解析后复用对象。
- **27-2083**：写 DDR 必须选 64 位 jtag-axi 核；板级脚本已自动选择。
- **44-49**：不要给 hw_target 设 frequency。
- 同一时刻只允许一个 Vivado 会话；下载线接 CN10。

## 串口

- 115200 8N1；16550 @ `0x310b0000`，30 MHz 下 divisor 16。
- 程序必须自己写 `LCR=0x83 → DLL=16, DLM=0 → LCR=0x03`；16550 复位默认
  分频 162 是 9600，会全静默。
- `stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb -crtscts -echo raw`；
  GNU `stty raw` 不清 echo，必须显式 `-echo`。
- FT232R 判别：`udevadm info -q property -n /dev/ttyUSBx | grep ID_MODEL`。

## 预期输出

- FFT：`CHECK: PASS`；ext 镜像还有 `EXT instruction demo: PASS`。
- MIMO：`MIMO_VALIDATE_PASS` + `MIMO_DONE status=0`。
- DDR 检查：`DDR16G_PASS`。
- 每个 AM 镜像正常结束：`Exit with code = 0`。

## 排障顺序

1. `flows/08_board_diag.sh` 排除时钟/复位/串口链路。
2. `tools/read_probes.tcl` dump 全部 VIO 探针（CPU 取指/访存、复位链）。
3. 检查 `FPGA_BIT_HOME` 是否指向当前 bit/ltx，避免旧探针文件。
4. 串口无输出时先确认 `CPPFLAGS=-DUART16550=1` 且 AM 重建过。
