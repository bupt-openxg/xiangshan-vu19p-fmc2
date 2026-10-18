# 升级到 Kunminghu V3（kmh-v3 分支）

> 本文记录把本工程从 Kunminghu V2 升级到 V3 的调研结论、适配改动与验证状态。
> V2 的完整实测记录在 `kmh-v2` 分支（冻结），V3 的差异在此文档中单独说明。

## 1. 版本选择（2026-10-17 调研）

| 项 | 结论 |
|---|---|
| V3 是否冻结 | **否**。香山仓库默认分支已改为 `kunminghu-v3`（`main`/`master` 已不存在），无 V3 tag/release，README 自述"仍在快速演进、建议下游优先 V2" |
| 采用的提交 | **`893e1580b705d81f113c30c7a05734e09a08050e`**（kunminghu-v3 @ 2026-10-16，`perf(param): enlarge ROB size to 160`） |
| 备选回退点 | `6714a69d`（2026-10-11，已核对 XSTop 端口与本提交一致） |
| 官方 Linux 镜像 | **无 V3 镜像**。XiangShan-doc 仅有 V2 的 `data_kmh.zip`（5,178,920 B，平台名 `bosc,kmh-v2-1core`）；FPGA 文档仍停留在 V2（最后更新 2025-02-25） |
| FPGA 配置 | `CONFIG=MinimalConfig`（V3 仍存在该名字；内含 OpenLLC 4MB + L2 128KB + L1D 32KB）。无 `KunminghuV3Config`；`FpgaDiffMinimalConfig` 存在但带 Deprecated 警告 |

## 2. V2 → V3 的工程差异（与适配）

| 差异点 | V2（4b9ddb8） | V3（893e158） | 我们的处理 |
|---|---|---|---|
| mill | 0.12.3（自包含 jar） | **0.12.17**（Maven 只有官方启动器 `.sh`/`.exe`） | `setup/02` 双策略：有 jar 用 jar，无 jar 下载官方启动器（按 `.mill-version` 自动缓存） |
| 顶层 submodule | 10 个（coupledL2/openLLC/huancun/fudian/aia…） | **9 个**：rocket-chip、difftest、ready-to-run、utility、yunsuan、**ChiselAIA**、**ChiselIOPMP**、**XSCache**、**OpenIOMMU** | `flows/02` 更新清单 |
| 嵌套 submodule | rocket-chip/{cde,hardfloat}、openLLC/openNCB | rocket-chip/{cde,hardfloat}、**XSCache/{OpenNCB,ZhuJiang}** | `flows/02` 更新（只初始化 build.mill 直接引用的部分） |
| 构建文件 | `build.sc` | **`build.mill`** | 无影响（make 封装） |
| XSTop 端口 | 165 个，含 `io_riscv_halt_0`、`dma_*`（37） | 167 个：**删除 `io_riscv_halt_0`/`dma_*`，新增 `io_riscv_wfi_0`、traceCoreInterface 的 `mstatus/valid`** | **补丁 0013**：RTL 端口自动探测 + `ifdef CPU_XIANGSHAN_KMHV2`（V2 原连接 / V3 用 WFI 作为 halt 观测位）；`dma_*` 由 `CONFIG_SIMTOP_HAS_DMA` 自动 tie-off |
| ClockGate | 生成 `ClockGate.v` | 生成 **`ClockGate.sv`**、例化 11 处 | `patch_rtl.sh` 两代都处理（保持 TE/E/CK/Q 端口） |
| SRAM 数组 | `array_*.v` + URAM 提示 | 无 `array_*.v`（SRAM 模板） | URAM 提示步骤自然跳过 |
| DPI 包装 | 只生成 `DummyDPICWrapper` 例化、无模块定义 → INBB-3 黑盒 | 同时生成 `DummyDPICWrapper_*.sv`（21）+ `DiffExt*.v`（21），DPI import 均在 `ifndef SYNTHESIS/ifdef DIFFTEST` 保护内 | `patch_rtl.sh` 检测到模块定义时**保留**例化；残留检查仍逐项通过 |
| 生成 RTL 规模 | 1579 文件 / 868MB | **1895 文件 / 1.3GB** | 工程文件清单 1893 个 RTL（另 2 个非 RTL） |

## 3. 复现步骤（kmh-v3 分支）

```bash
flows/00_host_check.sh
flows/01_setup_toolchain.sh      # mill 按 .mill-version 自动选 0.12.17（官方启动器）
flows/02_clone_deps.sh           # 拉取 V3 @893e158 + submodule（9 顶层 + 4 嵌套）
flows/03_gen_rtl.sh              # make verilog CONFIG=MinimalConfig（30-60 分钟）
flows/04_build_bitstream.sh      # 建工程 + 综合 + 实现（约 7-10 小时）
flows/05_program_fpga.sh && flows/06_run_hello.sh
```

## 4. Linux 镜像（本轮暂缓）

官方无 V3 镜像。后续候选路径（按优先级）：

1. **V2 镜像 + DTB 适配**：先用 `data_kmh.zip` 在 V3 上试启动；若失败，用 V3 `make verilog`
   生成的 `build/XSTop.dts`（含 clint/plic/AIA 地址、CPU ISA 扩展等）与镜像内嵌 DTB 对比，
   对**字节长度不变**的属性（时钟频率、地址单元、兼容串）原地补丁后再试。
   注：V3 的 CPU ISA 扩展在 V2 基础上新增 `v`/`zfa`/`zvfh` 等，不影响既有内核运行。
2. **自建镜像**：按官方 Linux 构建流程重编 OpenSBI（使用 V3 DTS）+ 复用/重建 kernel 与 initramfs。

> 注意：DDR 载入点 0x80000000、UART 0x310B0000（在 XSTop 之外的 AXI_bridge/16550）、
> 25MHz UART 时钟与分频 14 等板级约定在 V3 中**未变**，裸机 Hello 无需任何修改。

## 5. 构建机要求（实测踩坑）

- **V3 stock MinimalConfig 的综合 elaboration 工作集 ≈ 110GB+**（OpenLLC 4MB + 8-wide 前后端 + 向量单元）。
- 在 15GB RAM 笔记本上的实测：
  - swap 63GB → **OOM**（Vivado 与其父进程一起被杀）；
  - swap 128GB → 不再 OOM，但 **磁盘抖动停滞**（CPU 86–92% 空闲、数十分钟日志零增长）；
  - 曾试验 `L3_CACHE_SIZE=1024`（1MB LLC）期望降低内存，**未缓解**（同一阶段仍 ~104GB）
    —— 主因是 V3 核心本身，不是 LLC；该妥协已撤销，**保持 stock `CONFIG=MinimalConfig`**。
- 结论：**V3 构建机需要 ≥64GB 物理内存**（推荐 128GB；swap ≥64GB 辅助）。
  swap 无法替代物理内存。构建期间不要运行 VirtualBox 等大内存程序。
- 中断恢复：综合被 OOM/终止后 `reset_runs synth_1` 重跑；实现阶段（opt/place/phys_opt 已完成、
  route 中断）可用 `reset_runs -prev_step impl_1` 保留检查点后
  `launch_runs impl_1 -to_step write_bitstream` 续跑。

## 6. 验证状态

- ✅ 依赖拉取（V3 clone + 9 顶层/4 嵌套 submodule 精确对齐）
- ✅ V3 RTL 生成（**stock `CONFIG=MinimalConfig`**）+ 模拟器残留检查通过 + 后处理（ClockGate.sv 透传）
- ✅ V3 工程创建（0 ERROR；端口自适应日志确认 DMA 缺失、halt 缺失→WFI）
- ▶️ 综合 / 实现 / 比特流 / 上板 Hello：见 README §四（随实测更新）
- ⏸️ Linux：本轮暂缓（无官方 V3 镜像）
