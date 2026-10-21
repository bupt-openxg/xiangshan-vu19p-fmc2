# XiangShan XVBBP on PHINE PD-VU19P-2-LSI SOLO v3

把香山昆明湖 `MinimalConfig` 单核 + XVBBP 7 条自定义 Q15 复数向量指令部署到
PHINE PD-VU19P-2-LSI SOLO v3 开发板，运行 BUPT `riscv-xvbbp` 仓库的 FFT/MIMO
裸机负载。

本版（2026-10-21）：

- **CPU 核 25 MHz**，16GB 1Rx8 DDR4，MIG 1600 MT/s。
- 依赖全部来自官方仓库；唯一的非官方库是 `bupt-openxg/riscv-xvbbp`。
- XVBBP 用 **overlay 模式**（不是 patch 模式）。
- **服务器只做综合/实现/比特流**；源码、RTL 生成、软件构建、烧录、测试都在本机。
- 测试只保留 BUPT 仓库出现的负载：48 FFT + 12 MIMO。

## 1. 版本元组

| 组件 | 版本 / 方式 |
|---|---|
| XiangShan | `OpenXiangShan/XiangShan@0a150819` |
| XiangShan 改动 | BUPT `hardware/xiangshan/overlay` 4 个 decode 文件 + `overlays/local/xiangshan/VIMacU.scala` 两行 `isComp/isConj` glue |
| YunSuan | `OpenXiangShan/YunSuan@996b51d` |
| YunSuan 改动 | BUPT `hardware/yunsuan/overlay` 10 个执行文件 |
| nexus-am | `OpenXiangShan/nexus-am@086a238` + `overlays/board/nexus-am` |
| env-scripts | `OpenXiangShan/env-scripts@876f39e` + `patches/env-scripts` 13 个板卡补丁 + `overlays/board/env-scripts` 16GB DDR |
| 负载源 | `bupt-openxg/riscv-xvbbp@c553928`（唯一非官方库） |
| 配置 | `MinimalConfig`（8-wide / ROB48 / L1D 32KB / L2 128KB×2 / L3 4MB） |
| 器件 | `xcvu19p-fsva3824-1-e` |

XiangShan 与官方基线只差 5 个源文件 + 1 个 yunsuan gitlink；YunSuan 只差 10 个文件。
仓库本身不复制 BUPT 的文件，`flows/02` 在构建时从 `third_party/riscv-xvbbp` 覆盖。

## 2. 时钟配置

| 时钟 | 频率 | 引脚 | 约束 / 用途 |
|---|---|---|---|
| `clk5` / CPU 系统 | **25 MHz** | BA51/BB51 | `DEBUG_CLK_IN` 40.000 ns；`sys_clk_i = dbg_clk_buf`；CPU 核和系统总线 25 MHz |
| `clk7` / DDR 参考 | **80 MHz** | BM44/BN44 | MIG `InputClockPeriod=12500`；MIG UI 200 MHz，PHY 1600 MT/s |
| `clk8` / RTC | **1 MHz** | E57/E58 | `TMCLK` 1000.000 ns |
| `clk6` / 板载 | **200 MHz** | BW37/BY37 | `CPU_CLK_IN` 5.000 ns；给 `cqetmclk`/CRG，不是 CPU 核时钟 |
| JTAG | 12 MHz | CN10 | `jtag_vclk` 83.333 ns |

- AEConsole：FMC2 **VADJ = 1.2 V**；PLL `GC02 = 80 MHz`、`GC04 = 25 MHz`、
  `GC06 = 1 MHz`。
- **别把 `CPU_CLK_IN=200MHz` 当 CPU 频率**：它是 `clk6`，只给 CRG/ETM。
- **别把 `mmcm_clkout5=50MHz` 当 CPU 频率**：它是 DDR MIG 的内部输出。
- UART：16550 @ `0x310b0000`，115200 8N1；25 MHz 下 divisor = 14。

## 3. DDR 配置（16GB 1Rx8）

| 项目 | 值 |
|---|---|
| 内存条 | 16GB 1Rx8 DDR4 SODIMM（1 rank × 8 颗 16Gb x8） |
| MIG part | `MT40A2G8VA-062E`（Components 模式） |
| 数据宽度 / rank | 64-bit / 1 rank，无 ECC，无 clamshell |
| MIG 速率 | 1600 MT/s，`TimePeriod=1250`，`InputClockPeriod=12500`，CL/CWL 11 |
| AXI | `DDR_AXI_ADDR_WIDTH=34`，地址范围 `0x000400000000`（16GB） |
| 引脚 | `A[16:0]`、`BA[1:0]`、`BG[1:0]`、64 DQ、8 DM、8 DQS；与 8GB 1Rx8 相同 |

配置入口是 `overlays/board/env-scripts/.../xs_uart.tcl` 的 1-rank 分支和
`jtag_ddr_subsys.tcl` 的 `DDR4_MemoryType`。`DDR_RANK_WIDTH` 仍为 **1**；
2 表示 16GB 2Rx8，不是本内存条。

## 4. 目录

```
ReConstruct/
├── flows/                 00 环境检查 · 01 工具链 · 02 依赖+overlay · 03 RTL
│                          04 打包服务器输入 · 05 烧录 · 06 裸机 Hello · 08 板卡诊断
│                          09 构建 BUPT 负载 · 10 运行 BUPT 负载
├── overlays/
│   ├── board/nexus-am/    串口 16550 · mstatus.VS · lp64d
│   ├── board/env-scripts/ 16GB DDR / JTAG MIG 配置
│   └── local/xiangshan/   VIMacU isComp/isConj glue
├── tests/
│   ├── bupt/              build.sh · run.sh · ddr16_check.tcl
│   └── common/            单次上板启动 + 串口抓取
├── tools/                 env.sh · setup/ · patch_rtl.sh · checks/ · build_server.tcl
├── hw/                    引脚表与板卡诊断
├── patches/env-scripts/   板卡移植 13 个补丁
├── docs/                  板卡/构建/证据文档
└── build/                 (gitignored) RTL、负载、日志、bit/ltx
```

## 5. 复现步骤

### 5.1 本机环境

要求：Linux x86_64、Vivado 2024.2（仅烧录和 JTAG 需要）、JDK 21、
xPack GNU RISC-V GCC 15.2、`zstd`、`git`、`python3`。

```bash
flows/00_host_check.sh
flows/01_setup_toolchain.sh
```

### 5.2 拉依赖并应用 overlay

```bash
flows/02_clone_deps.sh
```

脚本会：

1. 浅拉官方 XiangShan `0a150819`，初始化官方 submodule；
2. 把 yunsuan 固定到官方 `996b51d`；
3. 复制 BUPT 4 + 10 个 overlay 文件，再复制 `VIMacU.scala` glue；
4. 在 yunsuan 和 XiangShan 内各做一次本地提交，保证工作树干净；
5. 校验 XiangShan 相对官方只改 5 个源文件；
6. 拉官方 nexus-am `086a238` 并覆盖 3 个板级文件；
7. 拉 `riscv-xvbbp@c553928`；
8. 拉官方 env-scripts、应用 13 个板卡补丁、覆盖 16GB DDR 配置。

### 5.3 本机生成 RTL

```bash
flows/03_gen_rtl.sh
```

等价于：

```bash
cd third_party/XiangShan
make verilog CONFIG=MinimalConfig JVM_XMX=20G
bash ../../tools/patch_rtl.sh build/rtl
bash ../../tools/checks/rtl_check.sh build/rtl ../../build/check/rtl-check-report.md
```

检查点：`build/rtl` 中应能 grep 到 `io_isComp/io_isConj` 和 7 条指令编码；
`rtl-check-report.md` 通过。

### 5.4 本机打包（服务器只收综合输入）

```bash
flows/04_package_server.sh
```

产物在 `build/bupt-server-<date>/`：

- `rtl.tar.zst`（已包含 `DifftestMacros.svh`，已去掉 `*.fir/*.csv/*.anno.json`）
- `env-scripts-src.tar.zst`（已去掉 Vivado 工程、`.Xil`、日志）
- `build_server.tcl`、`SHA256SUMS`

上传到服务器：

```bash
scp -P 22103 build/bupt-server-<date>/{rtl.tar.zst,env-scripts-src.tar.zst,build_server.tcl,SHA256SUMS} \
    turing@10.156.64.60:~/.zhaoleo/xvbbp-buptopenxg/
```

服务器上只有这些输入，**没有完整仓库、没有工具链、没有软件源码**。

### 5.5 服务器建工程 + 综合 + 实现 + 比特流

```bash
cd ~/.zhaoleo/xvbbp-buptopenxg
sha256sum -c SHA256SUMS
tar --use-compress-program='zstd -d' -xf rtl.tar.zst -C rtl
tar --use-compress-program='zstd -d' -xf env-scripts-src.tar.zst -C env-scripts --strip-components=1

source ~/zhaoleo/Xilinx/Vivado/2024.2/settings64.sh
cd env-scripts/fpga_diff
make project CPU=kmh NO_DIFF=1 DDR_RANK_WIDTH=1 \
     CORE_DIR="$HOME/.zhaoleo/xvbbp-buptopenxg/rtl"
vivado -mode batch -source ../../build_server.tcl -tclargs \
     "$PWD/fpga_vivado_kmh-nodiff/fpga_vivado_kmh-nodiff.xpr" \
     "$HOME/.zhaoleo/xvbbp-buptopenxg/out" 16
```

`build_server.tcl` 只跑 `synth_1` → `impl_1 -to_step write_bitstream`，
再补写 timing/route/DRC/utilization 四份报告。`write_bitstream` 前置的
`report_power` 仍会执行一次（本轮 125GB 机器上用时约 5.5 分钟，峰值约 30GB），
本脚本不再额外调用。完成标记是 `=== SERVER BUILD DONE ===`。

工程日志里应确认：

```text
DDR_RANK_WIDTH=1; DDR_AXI_ADDR_WIDTH=34; DDR4_MEMORY_TYPE=Components;
DDR4_MEMORY_PART=MT40A2G8VA-062E
```

### 5.6 回传、烧录

```bash
mkdir -p artifacts/xvbbp-buptopenxg
scp -P 22103 turing@10.156.64.60:~/.zhaoleo/xvbbp-buptopenxg/out/fpga_top_debug.{bit,ltx} \
    artifacts/xvbbp-buptopenxg/
FPGA_BIT_HOME="$PWD/artifacts/xvbbp-buptopenxg" flows/05_program_fpga.sh
```

烧录成功标记：`=== 比特流烧录完成 ===`。
同一时刻只能有一个 Vivado 会话占用 JTAG。

### 5.7 DDR 16GB 检查

```bash
FPGA_BIT_HOME="$PWD/artifacts/xvbbp-buptopenxg" \
vivado -mode batch -source tests/bupt/ddr16_check.tcl \
    -tclargs "$PWD/artifacts/xvbbp-buptopenxg/fpga_top_debug.ltx"
```

在 2/6/10/14GB 和 16GB 末尾 8 字节写入不同 pattern 再读回；全对打印
`DDR16G_PASS`。地址超过 8GB 的窗口能排除“只识别成 8GB”的情况。

### 5.8 构建并运行 BUPT 负载

```bash
flows/09_build_bupt_sw.sh     # 48 FFT + 12 MIMO → build/bupt-<date>/images.tsv
FPGA_BIT_HOME="$PWD/artifacts/xvbbp-buptopenxg" flows/10_run_bupt.sh
```

`flows/10` 每个镜像执行一次 `halt → JTAG 写 DDR → VIO 复位 → 串口抓取`。
判定：

- FFT：`CHECK: PASS`；`ext` 镜像还要 `EXT instruction demo: PASS`。
- MIMO：`MIMO_VALIDATE_PASS` 且 `MIMO_DONE status=0`。

最后一行 `BUPT_RESULT total=60 pass=.. fail=..`。

## 6. 本版实测结果

### 6.1 16GB DDR

`tests/bupt/ddr16_check.tcl` 在 2/6/10/14GB 和 16GB 末尾 8 字节写入不同 pattern
后全部读回，结果：

```text
READ 0x80000000  = 0x1122334455667788 PASS
READ 0x180000000 = 0x99aabbccddeeff00 PASS
READ 0x280000000 = 0x0123456789abcdef PASS
READ 0x380000000 = 0x55aa55aa55aa55aa PASS
READ 0x3fffffff8 = 0xfedcba9876543210 PASS
DDR16G_PASS
```

### 6.2 BUPT 负载（60/60 PASS）

| 类别 | 数量 | 结果 |
|---|---:|---|
| FFT scalar | 16 | PASS |
| FFT RVV | 16 | PASS |
| FFT EXT | 16 | PASS |
| MIMO scalar | 4 | PASS |
| MIMO RVV | 4 | PASS |
| MIMO EXT | 4 | PASS |
| **合计** | **60** | **PASS 60 / FAIL 0** |

- EXT FFT 的 7 条指令逐条 PASS：`vscmul`、`vscmulcj`、`vscmacc`、
  `vscmacccj`、`vscaddconj`、`vscaddrq`、`vscaddrtq`，并打印
  `EXT instruction demo: PASS`。
- FFT 验证模式对比 OAI 参考，全部 `CHECK: PASS`，正常结束 `Exit with code = 0`。
- MIMO 每个 kernel 均 `MIMO_VALIDATE_PASS` + `MIMO_DONE status=0`；
  EXT 4×4 ZF 的 `fail=0`、`max_abs=10`、`over_tol=0`。
- 60 个镜像顺序上板，从 20:06 到 20:20，约 14 分钟（含 60 次 Vivado 会话）。

### 6.3 实现结果

| 指标 | 值 |
|---|---|
| 综合 | 57 分钟，0 errors / 0 critical warnings |
| 实现 | 2 小时 48 分钟到 `write_bitstream Complete!` |
| WNS / TNS | **+0.379 ns / 0.000 ns**（0 failing endpoints） |
| WHS / THS | **+0.010 ns / 0.000 ns**（0 failing endpoints） |
| WPWS / TPWS | +0.124 ns / 0.000 ns |
| 路由错误 | 0（2,343,203 routable nets 全部 routed） |
| DRC | 0 errors，170 warnings（LUT 方程/拥塞类，不阻塞） |
| CLB LUTs | 1,771,122 / 4,085,760 = **43.35%** |
| CLB Registers | 686,582 / 8,171,520 = **8.40%** |
| Block RAM | 469 / 2,160 = **21.71%** |
| URAM | 0 / 320 |
| DSPs | 3 / 3,840 |
| Bonded IOB | 136 / 2,072 |
| BUFGCE | 37 / 976 |
| PLL / MMCM | 3 / 1 |

交付物（本机 `artifacts/xvbbp-buptopenxg/`，不入库）：

```text
fpga_top_debug.bit   sha256 f76a42dd71df4e963dc68b04c32838b76fd0419661283da6829370eaed0b2896
fpga_top_debug.ltx   sha256 4c3268543da99a73d5fde0abef35b3c207e1c44a3674c13db3d23e7d0bd36e95
fpga_top_debug_{timing_summary,route_status,drc,utilization}_routed.rpt
```

### 6.4 本轮修复

- `tests/bupt/ddr16_check.tcl` 的 `axi_read` 用 `[get_hw_axi_txns rd_txn]`
  取事务对象后再读 `DATA`；同时先停 CPU、先释放 DDR 复位。
- `tests/common/board_boot.tcl` 在 JTAG 写 DDR 之前先 `sw4=1`、`sw5=0`、`sw6=0`。
- `flows/04` 的 `zstd` 加 `-f`，可重复打包。


## 7. 常见问题

| 现象 | 原因 / 处理 |
|---|---|
| 综合报 `cannot open include file 'DifftestMacros.svh'` | `cpu_files.tcl` 的 include_dirs 为空；`flows/04` 已把该文件放进 `rtl/rtl/` |
| 串口无输出 | 确认 16550 @ `0x310b0000`、`CPPFLAGS=-DUART16550=1`、divisor=14、115200 8N1 |
| FFT 跑几次后卡死 | `oai_dfts.o` 必须 `-march=rv64gc`，不能把 SIMDe 的 SSE dft16 编成 RVV；`tests/bupt/build.sh` 已处理 |
| MIMO 编译失败 | 必须用 xPack GCC 15（GCC 13 不认 4 参数 `vssra`）；ABI 为 lp64d |
| DDR 只认到 8GB | 检查 MIG part 是否为 `MT40A2G8VA-062E`、AXI 是否为 34 位、`DDR_RANK_WIDTH=1` |
| JTAG 报 27-2083 | 选 64 位 JTAG-AXI 核；板级脚本已自动选择 |
| 服务器 build 被 oomd | 不要在本机跑 synth/impl；服务器 125GB、16 jobs 峰值约 34GB。`report_power` 是 impl 前置步骤，本轮正常完成 |
| 工作树 dirty 导致 RTL 版本号异常 | `flows/02` 会在 XiangShan/YunSuan 内提交 overlay，必须保持 clean |

## 8. 来源与许可

- XiangShan / YunSuan / nexus-am / env-scripts：Apache-2.0，各自保留原许可。
- `bupt-openxg/riscv-xvbbp`：XVBBP 指令、执行单元和 FFT/MIMO 负载的来源；
  本仓库不复制其文件，构建时按 `c553928` 读取。
- XVBBP 设计与实现归 Cai Chi 等；板卡移植、16GB DDR 配置和本 README 的复现流程
  由本仓库维护。
