# 运行指南：Hello World 与 Linux

> **最快路径**：仓库根目录依次执行 `flows/05_program_fpga.sh` → `flows/06_run_hello.sh`
> → `flows/07_run_linux.sh`（本文档为对应的手工分步说明）。
>
> **适用对象**：板卡已上电、JTAG 与串口已连接（硬件连接/时钟配置/首次烧录见
> [`onboard-runbook.md`](onboard-runbook.md)）。
> **特点**：全程 Vivado TCL / VIO 控制，**不需要使用板卡上的任何按键/开关**。
>
> **实测状态（2026-10-15）**：v3 比特流 + 修正版镜像，Hello World 与官方 Linux
> 镜像均已跑通；串口统一 **115200 8N1**。

---

## 0. 一次性准备

### 0.1 载入环境（每次操作前）

```bash
cd <本仓库根目录>        # 克隆获得, 例如 ~/ReConstruct
source tools/env.sh
```

### 0.2 确认 / 烧录比特流

产物：`build/fpga_top_debug.bit` + `.ltx`（本机构建；或把他人提供的 Releases 产物放入 `build/`）。

板卡若已烧录可跳过；需要烧录（约 3 分钟）时：

```bash
vivado -mode tcl -source tools/prog_bitstream.tcl
```

### 0.3 串口

- 适配器为 **FT232R**（官方 Console 线），设备号可能是 `/dev/ttyUSB0` 或 `/dev/ttyUSB1`，
  用以下命令甄别：

  ```bash
  for d in /dev/ttyUSB*; do echo "$d: $(udevadm info -q property -n $d | grep -m1 ID_MODEL=)"; done
  # FT232R_USB_UART 的那个即是；Digilent JTAG 线(FT232H)不占 ttyUSB
  ```

- 参数：**115200 8N1**（`cs8 -cstopb -parenb -crtscts raw`）。
- 下文命令默认用 `/dev/ttyUSB0`，如不同请替换。

---

## 1. 运行 Hello World

### 1.1 操作（三步）

```bash
cd <本仓库根目录> && source tools/env.sh
V=$PWD/build
cd $FPGA_ROOT          # = third_party/env-scripts/fpga_diff（flows/02 拉取）

# ① 暂停 CPU（VIO sw6=0）
make halt_soc FPGA_BIT_HOME=$V

# ② 把裸机程序写入 DDR 偏移 0（CPU 物理地址 0x80000000）
make write_ddr WORKLOAD=$XS_ROOT/sw/baremetal/hello.txt

# ③ 串口 115200 打开并开始抓包，然后释放 CPU（VIO sw5 上升沿）
stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb -crtscts -echo raw
cat /dev/ttyUSB0 &
make reset_cpu FPGA_BIT_HOME=$V
```

### 1.2 期望输出（约 1 秒后）

```
Hello, XiangShan! (MinimalConfig on PHINE PD-VU19P, 115200 8N1)
```

### 1.3 原理与注意

- 复位后 CPU 从 bootrom（0x10000000）执行 3 条跳转指令 → 跳到 0x80000000（DDR 内即刚写入的程序）。
- 该程序自带 UART 分频初始化（`LCR=0x83 → DLL=14, DLM=0 → LCR=0x03`，
  25MHz/16/14 ≈ 111.6kbps ≈ 115200）。**16550 复位默认分频=162（9600 波特），不初始化会乱码。**
- 自定义程序请用 `tools/bin/bin2fpgadata` 生成 `.txt`（`sw/baremetal/Makefile` 已内置）。

---

## 2. 运行 Linux

官方昆明湖内存镜像由 `sw/linux/fetch_image.sh` 从官方地址下载（13.3MB 内存映像，sha256 固定）。

### 2.1 第一步：生成修正版镜像（**必须，只需一次**）

官方镜像内嵌的 DTB 假定 UART 时钟为 50MHz，本设计实际为 25MHz；
不打补丁则串口波特率错乱（只能看到 OpenSBI 横幅且解码不出）。

```bash
cd <本仓库根目录> && source tools/env.sh
python3 tools/patch_linux_dtb_clock.py \
    sw/linux/data.txt sw/linux/data_patched.txt
# 输出: 已修改 clock-frequency: 50000000 -> 25000000
```

（修正版由 `tools/run_linux.sh` / `flows/07_run_linux.sh` 自动生成。）

### 2.2 操作（三步）

```bash
V=$PWD/build
cd $FPGA_ROOT          # = third_party/env-scripts/fpga_diff（flows/02 拉取）

# ① 暂停 CPU
make halt_soc FPGA_BIT_HOME=$V

# ② 写入镜像（13.3MB，约 55 秒）
make write_ddr WORKLOAD=$XS_ROOT/sw/linux/data_patched.txt

# ③ 串口 115200 抓包，然后释放 CPU
stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb -crtscts -echo raw
cat /dev/ttyUSB0 &
make reset_cpu FPGA_BIT_HOME=$V
```

### 2.3 期望输出（约 4 秒内完成）

```
OpenSBI v1.5.1-3-g0dfdb94
Platform Name             : bosc,kmh-v2-1core
...
Domain0 Next Address      : 0x0000000080400000      ← 跳转 Linux 内核
...
[    0.000000] Linux version 6.10.7-00060-g80dff865005d-dirty ... riscv64 GNU/Linux
[    1.019208] 310b0000.serial: ttyS0 at MMIO 0x310b0000 (irq = 0, base_baud = 1562500) is a 16550A
Starting syslogd: OK
Starting klogd: OK
Linux buildroot 6.10.7-... riscv64 GNU/Linux
#                          ← buildroot shell 提示符
```

镜像为 buildroot 系统，直接给 root shell（无登录密码）。

### 2.4 交互验证

```bash
# 另开终端（或等输出停止后）向串口写命令：
printf 'uname -a\r\n' > /dev/ttyUSB0     # → Linux buildroot 6.10.7-... riscv64 GNU/Linux
printf 'ls /\r\n'    > /dev/ttyUSB0      # → bin dev etc init lib ...
printf 'cat /proc/version\r\n' > /dev/ttyUSB0
```

> 提示：内核日志滚动时不要连续快速输入，会触发 `input overrun`（字符丢失）；
> 等提示符稳定后再逐条输入。

### 2.5 为什么需要 2.1 的补丁（一句话）

串口波特率 = 时钟 ÷ (16×分频)，软件按 DTB 的 `clock-frequency` 计算分频；
补丁把该值从 50MHz 改成实际的 25MHz 后，**OpenSBI、Linux 内核与自制裸机三者
统一为分频 14 → ~111.6kbps（终端设 115200 即可）**。内核日志中
`base_baud = 1562500`（=25MHz/16）即为该分频的来源。

---

## 3. 两者之间切换

不需要重新烧录比特流；重复对应章节的三步即可：

- Linux → Hello：执行 **§1.1**（会覆盖 DDR 低 4KB，其余镜像残留无影响）。
- Hello → Linux：执行 **§2.2**（约 55 秒写回完整镜像）。

---

## 4. 波特率与时钟对照表

| 部件 | 时钟 | 分频 | 实际波特率 | 终端设置 |
|---|---|---|---|---|
| 裸机程序（自制 hello） | 25MHz (clk5) | 程序自设 14 | 111.6k | **115200 8N1** |
| OpenSBI（补丁后） | 25MHz | 14（读 DTB 25MHz） | 111.6k | **115200 8N1** |
| Linux 内核（补丁后） | 25MHz | 14 | 111.6k | **115200 8N1** |
| 16550 复位默认值 | 25MHz | 162（IP 默认） | 9600 | — |

---

## 5. 常见问题速查

| 现象 | 原因 / 处理 |
|---|---|
| 串口完全无输出 | ① 终端不是 115200 8N1；② 打开的设备不是 FT232R（见 §0.3）；③ 忘记 `make reset_cpu`（CPU 仍在复位） |
| Hello 输出乱码 | 用了未初始化分频的旧程序；请用 `sw/baremetal/hello.txt` |
| Linux 只有 OpenSBI 横幅、之后无输出 | 镜像未打 DTB 补丁（见 §2.1） |
| Linux 日志里字符偶尔丢失 | 控制台无流控，输入过快；等提示符稳定后再输入 |
| `make write_ddr` 报 `27-2083 … LEN=128 SIZE=32` | 设计含两个 JTAG-AXI 核，脚本已自动选择 64-bit DDR 核；若报错请检查脚本打印的所选核 |
| `make` 目标报 JTAG 被占用 | 同一时刻只能一个 Vivado 会话占用 JTAG；关闭 GUI 或其它 `make` 后重试 |
| 想重新生成/修改裸机程序 | `source tools/env.sh && make -C sw/baremetal` |

---

## 6. 一键脚本

等价于上述手工步骤，可直接使用：

```bash
flows/06_run_hello.sh              # 运行 Hello World（可选参数: 串口设备, 自动构建负载）
flows/07_run_linux.sh              # 运行 Linux（自动下载镜像+时钟修正+启动）
flows/06_run_hello.sh /dev/ttyUSB1 # 指定串口（默认 /dev/ttyUSB0）
```

`flows/05_program_fpga.sh` 用于（重新）烧录 `build/` 下的比特流。
