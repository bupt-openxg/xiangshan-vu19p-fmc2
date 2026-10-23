# XiangShan XVBBP on PHINE PD-VU19P-2-LSI SOLO v3

本项目把香山昆明湖 `MinimalConfig` 单核处理器和 XVBBP 7 条自定义 Q15 复数向量指令
部署到 PHINE PD-VU19P-2-LSI SOLO v3 开发板，并在板上运行
[bupt-openxg/riscv-xvbbp](https://github.com/bupt-openxg/riscv-xvbbp) 的 FFT/MIMO
裸机负载。

本版本的主要目标：

- 16GB 1Rx8 DDR4 内存配置；
- CPU 核 25 MHz，DDR MIG 1600 MT/s；
- 使用官方 XiangShan / YunSuan / nexus-am / env-scripts，XVBBP 以 overlay 方式集成；
- 完整跑通 BUPT 的 48 个 FFT 和 12 个 MIMO 镜像，共 60/60 通过。

## 1. 版本矩阵

| 组件 | 版本 / 集成方式 |
|---|---|
| XiangShan | [OpenXiangShan/XiangShan](https://github.com/OpenXiangShan/XiangShan) `@0a150819` |
| XiangShan 改动 | BUPT `hardware/xiangshan/overlay` 的 4 个 decode 文件，加 `overlays/local/xiangshan/VIMacU.scala` 的 `isComp/isConj` glue |
| YunSuan | [OpenXiangShan/YunSuan](https://github.com/OpenXiangShan/YunSuan) `@996b51d` |
| YunSuan 改动 | BUPT `hardware/yunsuan/overlay` 的 10 个执行单元文件 |
| nexus-am | [OpenXiangShan/nexus-am](https://github.com/OpenXiangShan/nexus-am) `@086a238` + `overlays/board/nexus-am` |
| env-scripts | [OpenXiangShan/env-scripts](https://github.com/OpenXiangShan/env-scripts) `@876f39e` + 13 个板卡补丁 + `overlays/board/env-scripts` |
| 负载来源 | [bupt-openxg/riscv-xvbbp](https://github.com/bupt-openxg/riscv-xvbbp) `@c553928` |
| 处理器配置 | `MinimalConfig`：8-wide、ROB48、L1D 32KB、L2 128KB×2、L3 4MB |
| FPGA | `xcvu19p-fsva3824-1-e` |
| Vivado | 2024.2 |

XiangShan 相对官方基线只改动 5 个源文件和 1 个 yunsuan gitlink；YunSuan 只改动
10 个文件。仓库本身不复制 BUPT 的源码，`flows/02` 在构建时从
`third_party/riscv-xvbbp` 读取并覆盖。

## 2. 板卡配置

### 2.1 时钟

| 时钟 | 频率 | 引脚 | 用途 |
|---|---:|---|---|
| `clk5` / CPU 系统 | **25 MHz** | BA51/BB51 | `DEBUG_CLK_IN`，周期 40.000 ns；CPU 核和系统总线 |
| `clk7` / DDR 参考 | **80 MHz** | BM44/BN44 | MIG `InputClockPeriod = 12500`；UI 200 MHz，PHY 1600 MT/s |
| `clk8` / RTC | **1 MHz** | E57/E58 | `TMCLK`，周期 1000.000 ns |
| `clk6` / 板载 | **200 MHz** | BW37/BY37 | `CPU_CLK_IN`，供 `cqetmclk`/CRG，不是 CPU 核时钟 |
| JTAG | 12 MHz | CN10 | `jtag_vclk`，周期 83.333 ns |

AEConsole 配置：FMC2 **VADJ = 1.2 V**，PLL 输出 `GC02 = 80 MHz`、
`GC04 = 25 MHz`、`GC06 = 1 MHz`。

注意区分：

- `CPU_CLK_IN = 200 MHz` 是 `clk6`，不是 CPU 核频率；
- `mmcm_clkout5 = 50 MHz` 是 DDR MIG 的内部输出，也不是 CPU 核频率；
- CPU 核由 `clk5` 驱动，本版本为 **25 MHz**。

UART 使用 16550，基地址 `0x310b0000`，115200 8N1；25 MHz 下分频系数为 14。

### 2.2 DDR

| 项目 | 值 |
|---|---|
| 内存条 | 16GB 1Rx8 DDR4 SODIMM，1 rank × 8 颗 16Gb x8 |
| MIG part | `MT40A2G8VA-062E`，Components 模式 |
| 数据宽度 | 64-bit，无 ECC，无 clamshell |
| MIG 速率 | 1600 MT/s，`TimePeriod = 1250`，`InputClockPeriod = 12500`，CL/CWL 11 |
| AXI | `DDR_AXI_ADDR_WIDTH = 34`，地址范围 `0x000400000000`（16GB） |
| 引脚 | `A[16:0]`、`BA[1:0]`、`BG[1:0]`、64 DQ、8 DM、8 DQS |

配置位于 `overlays/board/env-scripts` 的 `xs_uart.tcl` 和 `jtag_ddr_subsys.tcl`。
`DDR_RANK_WIDTH` 必须保持 **1**；值 2 对应 16GB 2Rx8，不适用于本内存条。

## 3. 仓库结构

```
.
├── flows/                 00 环境检查 · 01 工具链 · 02 依赖与 overlay
│                          03 RTL 生成 · 04 综合输入打包 · 05 烧录
│                          06 裸机 Hello · 08 板卡诊断
│                          09 构建 BUPT 负载 · 10 运行 BUPT 负载
├── overlays/
│   ├── board/nexus-am/    串口 16550、mstatus.VS、lp64d ABI
│   ├── board/env-scripts/ 16GB DDR / JTAG MIG 配置
│   └── local/xiangshan/   VIMacU isComp/isConj glue
├── tests/
│   ├── bupt/              build.sh、run.sh、ddr16_check.tcl
│   └── common/            单次上板启动与串口抓取
├── tools/                 env.sh、setup/、patch_rtl.sh、checks/、build_server.tcl
├── hw/                    引脚表与板卡诊断
└── patches/env-scripts/   板卡移植补丁
```

## 4. 复现步骤

### 4.1 环境要求

- Linux x86_64；
- Vivado 2024.2（烧录、JTAG 调试以及综合/实现）；
- JDK 21、Mill；
- xPack GNU RISC-V Embedded GCC 15.2；
- `git`、`python3`、`zstd`、`curl`、`unzip`；
- 至少 80GB 可用磁盘空间；
- 综合/实现机器建议至少 64GB 内存。

### 4.2 获取源码并应用 overlay

```bash
flows/00_host_check.sh
flows/01_setup_toolchain.sh
flows/02_clone_deps.sh
```

`flows/02` 会执行以下操作：

1. 浅拉官方 XiangShan `0a150819` 并初始化官方 submodule；
2. 把 yunsuan 固定到官方 `996b51d`；
3. 复制 BUPT 的 4 个 XiangShan overlay 和 10 个 YunSuan overlay；
4. 复制 `overlays/local/xiangshan/VIMacU.scala`；
5. 在 yunsuan 和 XiangShan 内各生成一次 Git 提交，保证工作树干净；
6. 校验 XiangShan 相对官方只改 5 个源文件；
7. 拉取官方 nexus-am `086a238` 并应用板级 overlay；
8. 拉取 `riscv-xvbbp@c553928`；
9. 拉取官方 env-scripts，应用板卡补丁和 16GB DDR overlay。

### 4.3 生成 RTL

```bash
flows/03_gen_rtl.sh
```

该脚本等价于：

```bash
cd third_party/XiangShan
make verilog CONFIG=MinimalConfig JVM_XMX=20G
bash ../../tools/patch_rtl.sh build/rtl
bash ../../tools/checks/rtl_check.sh build/rtl ../../build/check/rtl-check-report.md
```

检查点：

- `build/rtl` 中存在 `io_isComp`、`io_isConj` 和 7 条 XVBBP 指令的编码；
- `build/check/rtl-check-report.md` 通过；
- XiangShan 和 YunSuan 工作树保持干净。

### 4.4 打包综合输入

```bash
flows/04_package_server.sh
```

产物在 `build/bupt-server-<date>/`：

- `rtl.tar.zst`：生成的 RTL，已包含 `DifftestMacros.svh`；
- `env-scripts-src.tar.zst`：修改后的 env-scripts，已排除 Vivado 工程和日志；
- `build_server.tcl`：综合/实现脚本；
- `SHA256SUMS`：校验和。

### 4.5 综合、实现与比特流

在综合机器上设置两个变量（按实际环境替换）：

```bash
export BUILD_HOST=user@synthesis-host
export BUILD_DIR=/path/to/xvbbp-build
```

确保综合机工作目录存在，并把打包产物复制过去：

```bash
ssh "$BUILD_HOST" "mkdir -p '$BUILD_DIR'"
scp build/bupt-server-*/* "$BUILD_HOST:$BUILD_DIR/"
```

在综合机器上执行：

```bash
cd "$BUILD_DIR"
sha256sum -c SHA256SUMS
mkdir -p rtl env-scripts out
tar --use-compress-program='zstd -d' -xf rtl.tar.zst -C rtl
tar --use-compress-program='zstd -d' -xf env-scripts-src.tar.zst \
    -C env-scripts --strip-components=1

export VIVADO_ROOT=/path/to/Vivado/2024.2
source "$VIVADO_ROOT/settings64.sh"
cd env-scripts/fpga_diff
make project CPU=kmh NO_DIFF=1 DDR_RANK_WIDTH=1 CORE_DIR="$BUILD_DIR/rtl"
vivado -mode batch -source ../../build_server.tcl -tclargs \
    "$PWD/fpga_vivado_kmh-nodiff/fpga_vivado_kmh-nodiff.xpr" \
    "$BUILD_DIR/out" 16
```

`build_server.tcl` 依次执行：

- `synth_1`；
- `impl_1 -to_step write_bitstream`；
- 输出 timing、route、DRC、utilization 报告；
- 写 `out/fpga_top_debug.bit` 和 `out/fpga_top_debug.ltx`。

综合完成后应确认工程配置为：

```text
DDR_RANK_WIDTH=1
DDR_AXI_ADDR_WIDTH=34
DDR4_MEMORY_TYPE=Components
DDR4_MEMORY_PART=MT40A2G8VA-062E
```

### 4.6 烧录

在烧录机上设置比特流目录（按实际环境替换）：

```bash
export BIT_DIR=/path/to/bitstream
mkdir -p "$BIT_DIR"
scp "$BUILD_HOST:$BUILD_DIR/out/fpga_top_debug.{bit,ltx}" "$BIT_DIR/"
FPGA_BIT_HOME="$BIT_DIR" flows/05_program_fpga.sh
```

烧录完成标志为：

```text
=== 比特流烧录完成 ===
```

同一时刻只允许一个 Vivado 会话占用 JTAG。

### 4.7 DDR 检查

```bash
FPGA_BIT_HOME="$BIT_DIR" \
vivado -mode batch -source tests/bupt/ddr16_check.tcl \
    -tclargs "$BIT_DIR/fpga_top_debug.ltx"
```

测试会在 2/6/10/14GB 和 16GB 末尾 8 字节分别写入不同 pattern 再读回。
全部通过时输出：

```text
DDR16G_PASS
```

### 4.8 构建并运行 BUPT 负载

```bash
flows/09_build_bupt_sw.sh
FPGA_BIT_HOME="$BIT_DIR" flows/10_run_bupt.sh
```

`flows/09` 生成 48 个 FFT 和 12 个 MIMO 镜像，写入
`build/bupt-*/images.tsv`。

`flows/10` 对每个镜像执行一次：

```text
halt -> JTAG 写 DDR -> VIO 复位 -> 串口抓取
```

判定标准：

- FFT：`CHECK: PASS`；`ext` 镜像还需要 `EXT instruction demo: PASS`；
- MIMO：`MIMO_VALIDATE_PASS` 且 `MIMO_DONE status=0`；
- 汇总：`BUPT_RESULT total=60 pass=60 fail=0`。

## 5. 测试内容与结果

### 5.1 测试矩阵

| 类别 | 后端 | 数量 |
|---|---|---:|
| FFT | scalar / RVV / EXT，各 8 点 × 正/逆变换 | 48 |
| MIMO | scalar / RVV / EXT，各 2×2/4×4 ZF/MMSE | 12 |
| 合计 | — | 60 |

FFT 覆盖 16、32、64、128、256、512、1024、4096 点。MIMO 覆盖
2×2 和 4×4 的 ZF/MMSE 均衡。

### 5.2 测试结果

| 类别 | 数量 | 结果 |
|---|---:|---|
| FFT scalar | 16 | PASS |
| FFT RVV | 16 | PASS |
| FFT EXT | 16 | PASS |
| MIMO scalar | 4 | PASS |
| MIMO RVV | 4 | PASS |
| MIMO EXT | 4 | PASS |
| **合计** | **60** | **60/60 通过** |

- EXT FFT 中 7 条指令逐条通过：`vscmul`、`vscmulcj`、`vscmacc`、
  `vscmacccj`、`vscaddconj`、`vscaddrq`、`vscaddrtq`；
- FFT 与 OAI 参考结果对比，全部 `CHECK: PASS`；
- MIMO 每个 kernel 均通过 `MIMO_VALIDATE_PASS`，`MIMO_DONE status=0`；

### 5.3 时序结果

按路径所涉及的 SLR 统计，routed 设计的 WNS：

| 时序 | WNS (ns) |
|---|---:|
| **总体 WNS** | **+0.379** |
| **总体 TNS** | **0.000** |

### 5.4 比特流


```text
fpga_top_debug.bit
SHA-256: f76a42dd71df4e963dc68b04c32838b76fd0419661283da6829370eaed0b2896

fpga_top_debug.ltx
SHA-256: 4c3268543da99a73d5fde0abef35b3c207e1c44a3674c13db3d23e7d0bd36e95
```

比特流和实现报告不纳入 Git 仓库，构建时在 `out/` 下生成。

## 6. 来源与许可

- XiangShan、YunSuan、env-scripts：木兰宽松许可证第 2 版（Mulan PSL v2）；
- nexus-am：遵循上游仓库的许可证；
- `bupt-openxg/riscv-xvbbp`：Apache-2.0；XVBBP 指令、执行单元和 FFT/MIMO
  负载的来源，本仓库不复制其文件，构建时按 `c553928` 读取；
- XVBBP 设计与实现归 Cai Chi 等作者所有；
- 板卡移植、16GB DDR 配置和复现流程由本仓库维护。
