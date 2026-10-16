# 上板实操手册 — 外部硬件连接 · 裸机 Hello World · Linux

> 适用: PHINE PD-VU19P-2-LSI SOLO v3 + 本工程生成的比特流
> （比特流见 `build/`，香山昆明湖 MinimalConfig）
> 前置阅读: [docs/board-setup.md](board-setup.md)（AEConsole 一次性配置详情）
>
> **只想跑程序？** 直接用精简版 [docs/run-guide.md](run-guide.md)
> （Hello World 与 Linux 的完整步骤；推荐直接用 `flows/06_run_hello.sh` / `flows/07_run_linux.sh`），
> 或跳至本手册 [§7](#7-运行裸机-hello-world) / [§8](#8-运行-linux)。

---

## 目录
1. [硬件总览与接口位置](#1-硬件总览与接口位置)
2. [外部硬件连接（分步）](#2-外部硬件连接分步)
3. [AEConsole 安装与一次性配置](#3-aeconsole-安装与一次性配置)
4. [烧录比特流（两种方式）](#4-烧录比特流两种方式)
5. [检查板卡状态](#5-检查板卡状态)
6. [打开串口终端](#6-打开串口终端)
7. [运行裸机 Hello World](#7-运行裸机-hello-world)
8. [运行 Linux](#8-运行-linux)
9. [故障排查速查表](#9-故障排查速查表)
10. [附录](#10-附录)

---

## 1. 硬件总览与接口位置

### 1.1 板卡接口一览

| 位置 | 接口 | 用途 | 本工程是否使用 |
|---|---|---|---|
| 后面板 | 电源插座 (C14, AC 220V) | 整机供电 | ✅ 必须 |
| 后面板 | **USB 3.0 Type-B** | 连接板卡控制器 (AEConsole)，USB 模式 | ✅ 必须（或用以太网）|
| 后面板 | ETH-A (RJ45, 千兆) | 连接板卡控制器，以太网模式 | 可选（与 USB 二选一）|
| 后面板 | ETH-B / MGM (RJ45) | 保留未支持 | ❌ |
| 后面板 | **User (RJ45)** | **用户串口 UART**（仅 TX/RX 两信号） | ✅ 必须 |
| 后面板 | ARM JTAG | 保留未支持 | ❌ |
| 主板 | **CN10 (AMD JTAG, 14-pin 2×7)** | FPGA 配置 + JTAG-to-AXI | ✅ 必须 |
| 主板 | CN43 (GPIO 排针 2.0mm) | 引出 RISC-V 调试 JTAG（可选） | 可选 |
| 主板 | **FMC2 (HPC 槽)** | DDR4 子卡 UV-FMCH-PDDR4DME | ✅ 必须 |
| 机内 | micro-SD 卡槽 | 比特流自动加载（AEConsole 写入） | 可选 |

### 1.2 随箱附带线材（交付清单）

- AMD **JTAG 下载线** ×1（主板 CN10 ↔ PC USB）
- **UART Console 线** ×1（后面板 User RJ45 ↔ PC）
- **USB3.0 Type-B 线** ×1（后面板 USB ↔ PC）
- RJ45 网线 ×1（可选，ETH-A）

### 1.3 CN10 JTAG 接口定义（标准 14-pin）

| 引脚 | 信号 | 引脚 | 信号 |
|---|---|---|---|
| 1/3/5/7/9/11/13 | GND | 2 | VREF (1.8V) |
| 4 | TMS | 6 | TCK |
| 8 | TDO | 10 | TDI |
| 12/14 | NC | | |

> 使用 AMD/Xilinx 或 Digilent 兼容下载线直插即可。

---

## 2. 外部硬件连接（分步）

> ⚠️ **顺序很重要**：首次使用必须先上电完成 AEConsole 配置（VADJ/时钟），断电后再插 DDR 子卡。

### 步骤 2.1 — 上电（暂不插 DDR 子卡）
1. 确认电源电压 AC 220V，电源线接入后面板；
2. 打开机箱后面板电源开关；确认面板 **PWR** 灯亮；
3. 等待板卡控制器（ZYNQ）启动完成（约 1 分钟）。

### 步骤 2.2 — 连接板卡控制器
- **USB 模式（推荐首次）**：USB3.0 Type-B 线连接后面板 USB ↔ PC；
- 或 **以太网模式**：网线连接 ETH-A ↔ PC，且 PC 与板卡控制器设在同一网段（后续在 AEConsole 选择 Ethernet 模式）。

### 步骤 2.3 — 安装并配置 AEConsole
见[第 3 节](#3-aeconsole-安装与一次性配置)。完成后**断开总电源**。

### 步骤 2.4 — 安装 DDR4 子卡
1. 断电状态下，将 **UV-FMCH-PDDR4DME 子卡**插入 **FMC2** 槽（HPC，注意方向与连接器对齐，拧紧固定螺丝）；
2. 将 **1Rx8 8GB DDR4 SODIMM** 插入子卡的 260-pin 插槽（斜插入后下压卡扣）；
3. 重新上电（保持 2.2 的连接）。

### 步骤 2.5 — 连接 JTAG 下载线
AMD JTAG 线接主板 **CN10**，另一端接 PC USB（首次使用需装驱动，见 9.1）。

### 步骤 2.6 — 连接串口
随箱 **UART Console 线**的 RJ45 端接后面板 **User** 口，另一端接 PC。
- PC 端若为 USB：确认出现 `/dev/ttyUSB0`（Linux）；
- 若需用通用 RJ45 转接板自制：仅 TX/RX 两信号（LVCMOS33，**勿接 5V**）。识别方法见[附录 A](#附录-a-串口-rj45-线序确认)。

### 步骤 2.7 — 最终检查
- [ ] PWR 灯亮、风扇转
- [ ] DDR 子卡已插紧在 FMC2
- [ ] JTAG 线接 CN10 ↔ PC
- [ ] 串口线接 User 口 ↔ PC
- [ ] USB3/网线接板控

---

## 3. AEConsole 安装与一次性配置

### 3.1 安装（Linux 上位机）
```bash
cd tools/aeconsole/ae-console
sudo bash install.sh      # 安装依赖 deb + 板控服务组件（需要 sudo 密码）
./ae-console              # 启动 GUI
```
（BSP 内为 Ubuntu 版 AEConsole 2.5.0；Windows 主机用 BSP 内 `Software/AEConsole/Windows/` 安装包）

### 3.2 连接板卡
启动后在 AEConsole 选择连接方式（USB 或 Ethernet），连接成功后可看到板卡状态。

### 3.3 一次性配置（**关键，缺一不可**）

**① FMC2 电压 = 1.2V**
左侧面板找到 FMC 电压设置 → `FMC2 VADJ = 1.2V`。（DDR4 电平，设置后持久保存）

**② Global Clock（PLL 全局时钟）** —— 左侧面板 `Global Clock`：

| 输入 | 输出通道 | 频率 | 去向 FPGA 引脚 | 用途 |
|---|---|---|---|---|
| OSC 54MHz | HP20_GC02 | **80 MHz** | BM44/BN44 | DDR4 MIG 参考时钟 |
| OSC 54MHz | HP26_GC04 | **25 MHz** | BA51/BB51 | CPU / 系统时钟 |
| OSC 54MHz | HP35_GC06 | **1 MHz** | E57/E58 | RTC (tmclk) |

点击 `Program` 写入。（若 1MHz 被拒，改 4MHz 并同步改 fpga.xdc 与重新生成比特流，见 board-setup.md 备注）

**③ 断电重新上电**，一次性配置生效。

> 以后每次开机前确认：FMC2 VADJ=1.2V、三路时钟配置在位（AEConsole 可回读）。

---

## 4. 烧录比特流（两种方式）

### 方式 A（推荐）：Vivado Hardware Manager + AMD JTAG

GUI 步骤：Vivado → Open Hardware Manager → Open Target → Auto Connect → 右键设备 → Program Device → 选 `build/fpga_top_debug.bit` + `build/fpga_top_debug.ltx`。

或命令行（等价，免 GUI）：
```bash
cd <本仓库根目录> && source tools/env.sh
vivado -mode tcl <<'EOF'
open_hw_manager
connect_hw_server
open_hw_target
# 无需设置 JTAG 频率: 默认值即线缆合法工作值
# (原脚本固定 12MHz 在 Digilent 线缆上报 [Labtoolstcl 44-49]; 如需调优可解除下行注释)
# set_property PARAM.FREQUENCY 15000000 [current_hw_target]
current_hw_device [lindex [get_hw_devices] 0]
refresh_hw_device [current_hw_device]
set_property PROGRAM.FILE  <本仓库根目录>/build/fpga_top_debug.bit [current_hw_device]
set_property PROBES.FILE   <本仓库根目录>/build/fpga_top_debug.ltx [current_hw_device]
program_hw_devices [current_hw_device]
exit
EOF
```
连接成功时应看到器件 **xcvu19p**（IDCODE 0x04BA1093）。

### 方式 B：AEConsole FPGA Bin Download（无需 JTAG 线）
1. AEConsole → `FPGA Configuration > FPGA Bin Download`；
2. 选择 `Config From PC` → `Select Download File` 选 **由 `build/fpga_top_debug.bit` 转出的 `.bin`**（`write_cfgmem` 生成）；
3. `Configure FPGA` 切到 Enable → 点 `Program`；
4. 可同时把 `Save to SD Card` 切 Enable 并勾选 `FPGA Config Auto`，以后上电自动加载。

> 注意：**运行例程（第 7/8 节）仍然需要 AMD JTAG 线**——向 DDR 写镜像依赖 FPGA 内的 JTAG-to-AXI，只能经 CN10 访问。

---

## 5. 检查板卡状态

烧写比特流后（约 5~15 秒 DDR 校准）：

| 板上 LED | 信号 | 期望状态 |
|---|---|---|
| **LED3** (AB13) | `init_calib_complete` | ✅ **点亮 = DDR 校准成功**（必须） |
| LED1 (AA16) | `cpu_wr_ddr_valid` | 运行负载时闪烁 |
| LED2 (AA14) | `cpu_rd_qspi_valid` | bootrom 取指时闪烁 |

若 LED3 不亮 → 检查 FMC2 VADJ=1.2V、三路 PLL 时钟、子卡接触（见 9.3）。

---

## 6. 打开串口终端

参数：**115200 bps, 8-N-1, 无流控**

Linux：
```bash
ls /dev/ttyUSB*                                  # 确认串口设备
sudo usermod -aG dialout $USER                   # 一次性（需重新登录）或: sudo chmod 666 /dev/ttyUSB*
picocom -b 115200 /dev/ttyUSB0                   # 或
minicom -D /dev/ttyUSB0 -b 115200                # 或
screen /dev/ttyUSB0 115200                       # 退出: Ctrl-A K
```

> **设备号甄别**（重要）：控制台适配器是 FT232R（`ID_MODEL=FT232R_USB_UART`），
> 重新插拔后编号可能变为 ttyUSB0 或 ttyUSB1。用
> `udevadm info -q property -n /dev/ttyUSBx | grep ID_MODEL` 确认；Digilent JTAG 线
> （FT232H）不占用 ttyUSB。建议固定别名：`/etc/udev/rules.d/99-ft232r.rules` 按序列号映射。
> 纯读捕获：`stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb -crtscts -echo raw && cat /dev/ttyUSB0`

Windows：PuTTY / MobaXterm → Serial，COMx，115200 8N1。

---

## 7. 运行裸机 Hello World

> 原理：CPU 复位后从 0x1000_0000 的 bootrom(BRAM) 执行，跳转到 0x8000_0000（DDR）。
> 因此先用 JTAG-to-AXI 把程序写入 DDR 偏移 0，再释放 CPU 复位。

### 7.1 环境准备（每次操作前）
```bash
cd <本仓库根目录>
source tools/env.sh
cd $FPGA_ROOT
export FPGA_BIT_HOME=$XS_ROOT/build            # 内含 fpga_top_debug.bit/.ltx
```

### 7.2 三步走（v3 实测通过，2026-10-15）

```bash
export FPGA_BIT_HOME=$XS_ROOT/build               # 本机构建的比特流

# ① 暂停系统（VIO sw6=0，同时停住 CPU）
make halt_soc FPGA_BIT_HOME=$FPGA_BIT_HOME

# ② 经 JTAG-to-AXI 写入 hello 程序（自制版，含 UART 分频初始化）
make write_ddr WORKLOAD=$XS_ROOT/sw/baremetal/hello.txt
#    注意: 必须用 bin2fpgadata 生成（make 自动调用）；勿手写十六进制

# ③ 串口 115200 8N1 打开（并开始抓包），释放复位 → CPU 启动
stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb -crtscts raw && cat /dev/ttyUSB0 &
make reset_cpu FPGA_BIT_HOME=$FPGA_BIT_HOME
```

预期输出：`Hello, XiangShan! (MinimalConfig on PHINE PD-VU19P, 115200 8N1)`

> ⚠️ 三个已知坑（详见 build-log.md）：① 16550 复位默认分频≈38400，程序必须自行写
> DLL=14/DLM=0（hello.c 已含）；② 官方 ready-to-run/hello-xs.bin 用的是
> 0x31B00000（与本设计 0x310B0000 不符）不可用；③ UART 寄存器 4 字节步长（LSR=+0x14）。

### 7.3 期望结果
串口终端（第 6 节已打开）应输出：

```
Hello, XiangShan!
```

### 7.4 运行自编译版本（可选）
```bash
cd $XS_ROOT/sw/baremetal
make                     # 本地 riscv64 工具链(env.sh 已配置) → hello.txt
cd $FPGA_ROOT
make write_ddr WORKLOAD=$XS_ROOT/sw/baremetal/hello.txt
make reset_cpu FPGA_BIT_HOME=$FPGA_BIT_HOME
# 输出: Hello, XiangShan! (MinimalConfig on PHINE PD-VU19P)
```

### 7.5 重复运行 / 切换负载
同一比特流下可反复执行 7.2 三步；换负载只需重新 `write_ddr` 对应 txt（写入会覆盖旧内容所在区域）。

---

## 8. 运行 Linux（v3 比特流实测通过，2026-10-15）

官方昆明湖内存镜像（`sw/linux/fetch_image.sh` 自动下载，13.3MB 内存映像）。
**注意：必须先用补丁脚本修正镜像 DTB 里的 UART 时钟假设**（官方镜像假定 50MHz，
本设计实际 25MHz），否则串口只能看到 OpenSBI 横幅且波特率错乱：

```bash
cd $XS_ROOT && source tools/env.sh
export FPGA_BIT_HOME=$XS_ROOT/build

# ① 生成修正版镜像（只需一次）
python3 tools/patch_linux_dtb_clock.py \
    sw/linux/data.txt sw/linux/data_patched.txt

# ② 暂停 CPU
cd env-scripts/fpga_diff
make halt_soc FPGA_BIT_HOME=$FPGA_BIT_HOME

# ③ 写入镜像（约 55 秒）
make write_ddr WORKLOAD=$XS_ROOT/sw/linux/data_patched.txt

# ④ 串口 115200 8N1 打开并抓包，然后释放 CPU
stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb -crtscts raw && cat /dev/ttyUSB0 &
make reset_cpu FPGA_BIT_HOME=$FPGA_BIT_HOME
```

### 期望结果（约 4 秒后，串口 115200 8N1）

```
OpenSBI v1.5.1 ... Platform Name : bosc,kmh-v2-1core
[    0.000000] Linux version 6.10.7-... riscv64 GNU/Linux
[    1.019208] 310b0000.serial: ttyS0 at MMIO 0x310b0000 (irq = 0, base_baud = 1562500) is a 16550A
Starting syslogd: OK / Starting klogd: OK
#                     <- buildroot shell 提示符, 可直接输入命令
```

实测交互（可直接向串口写命令）：
```bash
printf 'uname -a\r\n' > /dev/ttyUSB0     # -> Linux buildroot 6.10.7-... riscv64 GNU/Linux
printf 'ls /\r\n'    > /dev/ttyUSB0     # -> bin dev etc init lib ...
```
（镜像为 buildroot 系统，直接给 root shell，无登录密码；若用带登录的镜像则 root/bosc。）

### 波特率对照表（本工程最终配置）

| 部件 | 时钟 | 分频 | 实际波特率 | 终端设置 |
|---|---|---|---|---|
| 裸机程序（自制）| 25MHz (clk5) | 程序自设 14 | 111.6k | **115200 8N1** |
| OpenSBI（补丁后）| 25MHz | DTB 25MHz→14 | 111.6k | **115200 8N1** |
| Linux 内核（补丁后）| 25MHz | 同上 | 111.6k | **115200 8N1** |
| 16550 复位默认 | 25MHz | 162（IP 默认）| 9600 | — |

### 切回 Hello World

重复 7.2 三步（写入 `sw/baremetal/hello.txt`）即可，无需重新烧录比特流。

## 9. 故障排查速查表

### 9.1 JTAG 链找不到（Vivado 里没有 xcvu19p 器件）
| 检查项 | 处理 |
|---|---|
| USB 驱动 | Linux 安装 Xilinx cable drivers：`sudo $HOME/Xilinx/Vivado/2024.2/data/xicom/cable_drivers/lin64/install_script/install_drivers/install_drivers` |
| 线缆方向 | CN10 的 VREF(1.8V) 脚位对准，JTAG 线指示灯亮 |
| 板卡供电 | PWR 灯亮、风扇转 |
| hw_server 冲突 | **同一时刻只有一个 Vivado 会话能占用 JTAG**：先关闭 GUI 再跑 `make` 目标，反之亦然 |

### 9.2 `make write_ddr` 报错
- **`[Labtools 27-2083] Data value ... longer than ... LEN=128 and SIZE=32`**：
  设计含两个 JTAG-AXI 核（32-bit flash 用 / 64-bit DDR 用），若脚本抓错核即报此错。
  本工程脚本已修复为**按 `AXI_DATA_WIDTH` 自动选择 64-bit DDR 核**并打印所选核；
  如需手动指定：`make write_ddr HW_AXI_NAME=hw_axi_2 WORKLOAD=...`（名字以脚本打印的为准）。
- **找不到 AXI 核 / `No matching hw_axi`**：比特流未正确加载或加载的是其他工程
  → 重新烧写 `artifacts/fpga_top_debug.bit`；`.ltx` 与比特流不匹配
  → 确保 `FPGA_BIT_HOME=$XS_ROOT/artifacts`（bit/ltx 成对）。

### 9.3 LED3 (init_calib_complete) 不亮 — DDR 校准失败
按顺序检查：
1. AEConsole 中 **FMC2 VADJ = 1.2V**？改过后是否**断电重上电**？
2. 三路 PLL 时钟（80M/25M/1M）是否 Program 生效？
3. 子卡是否插紧在 **FMC2**、SODIMM 是否压紧卡扣？
4. 内存条是否 1Rx8 单 rank（与 MIG 配置一致）？
5. 断电重上电后重新烧写比特流再观察。

### 9.4 串口无输出
1. **确认执行了完整三步**（halt → write_ddr → reset_cpu）——hello 只在复位启动瞬间打印一次，
   终端**必须先打开**；看不到就重跑 `make reset_cpu FPGA_BIT_HOME=$FPGA_BIT_HOME` 再看；
2. **当前比特流（2026-10-13）每次 FPGA 重新配置后需先打 ROM 启动补丁**（coe 中的
   mnstatus CSR 指令与 4b9ddb8 不兼容，已在仓库修复 coe，但现有比特流需运行时补丁）：
   `bash tools/patch_bootrom.sh` 然后再 reset_cpu。重新综合的新比特流无此问题；
3. 波特率 115200 8N1、流控关闭；串口线接的是后面板 **User** 口（不是 MGM）；
4. 复位瞬间 LED2 (AA14) 应有一次取指闪烁；完全不闪 → 回 3.3 检查 PLL 时钟配置；
5. 仍是全静默 → 排查串口链路：确认 `/dev/ttyUSB*` 出现、尝试交换 TX/RX 两根线
   （接反会完全静默，最常见）、适配器须为 **3.3V TTL 电平**（RS-232 ±12V 电平不可直接接）。

### 9.5 `vivado: command not found`
未 source 环境：`source <本仓库根目录>/tools/env.sh`。

---

## 10. 附录

### 附录 A：串口 RJ45 线序确认
板卡**随箱附带的 UART Console 线**即为正确线序，直接使用。若需自行制作：
- User 口仅 2 个信号：UART_TX（FPGA 输出，BG14）、UART_RX（FPGA 输入，BH14），LVCMOS33；
- 识别方法：用 RJ45 转接板 + 万用表/示波器：**TX 空闲为高（3.3V）**；
- 禁止将该口对接 5V 电平设备；
- 连接器照片见 UG §4.1.6（PDF 第 100 页）。

### 附录 B：关键地址（软件/调试用）
| 区域 | 地址 |
|---|---|
| DDR（CPU 视角） | 0x8000_0000 起 |
| Bootrom（BRAM 仿真 QSPI） | 0x1000_0000 |
| UART16550 | 0x310B_0000 |
| SYS_CFG | 0x3120_0000 |
| data.txt 的地址 | DDR 偏移（0 = 0x8000_0000）|

### 附录 C：文件清单
| 文件 | 说明 |
|---|---|
| `artifacts/fpga_top_debug.bit` | 比特流（JTAG 烧写用）|
| `artifacts/fpga_top_debug.bin` | AEConsole Bin Download 用（SMAPx8）|
| `artifacts/fpga_top_debug.ltx` | 探针文件（VIO/写DDR 用，与 bit 成对）|
| （官方 ready-to-run 负载不适用本设计, 见 build-log 说明）| — |
| `sw/baremetal/hello.txt` | 自编译裸机 Hello World（`make -C sw/baremetal`）|
| `sw/linux/data.txt` | Linux 内存镜像（`sw/linux/fetch_image.sh` 下载）|

若 AEConsole 报 .bin 格式不匹配，可重新生成（其他接口变体）：
```bash
source tools/env.sh
vivado -mode batch -nojournal -nolog -source <(echo 'write_cfgmem -format BIN -interface SPIx8 -loadbit "up 0x0 '$PWD'/artifacts/fpga_top_debug.bit" -force /tmp/fpga_top_debug_spi.bin')
```

### 附录 D：上板检查清单（打印对照用）
- [ ] FMC2 VADJ=1.2V + 三路 PLL 时钟已 Program，且断电重上电过
- [ ] DDR 子卡（1Rx8 8GB）插在 FMC2
- [ ] JTAG 线接 CN10；串口线接 User 口；串口终端 115200 已打开
- [ ] 烧写 `artifacts/fpga_top_debug.bit` + `.ltx`
- [ ] LED3 点亮（DDR 校准完成）
- [ ] `make halt_soc` → `make write_ddr` → `make reset_cpu`
- [ ] 串口出现 `Hello, XiangShan!` / Linux 登录 `root` / `bosc`
