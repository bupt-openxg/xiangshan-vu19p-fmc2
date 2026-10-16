# 构建与调试日志（原始工程的完整记录）

> **说明**：本文是移植开发过程的完整调试记录，成文于原始开发工程（其目录结构与本仓库
> 不同）。叙述中出现的路径为**当时**的工程路径，在**本仓库（ReConstruct）**中的对应关系：
>
> | 原始工程 | 本仓库 |
> |---|---|
> | `artifacts/`、`artifacts/v3/` | `build/` |
> | `artifacts/v3/evidence/` | `docs/evidence/` |
> | `workloads/baremetal/` | `sw/baremetal/` |
> | `sw/linux/` | `sw/linux/` |
> | `env-scripts/fpga_diff/` | `third_party/env-scripts/fpga_diff/`（由 `patches/env-scripts/` 落地）|
> | `XiangShan/` | `third_party/XiangShan/` |
> | `tools/board_diag/` | `hw/diag/` |
>
> 复现入口以 [`README.md`](../README.md) 与 `flows/` 为准。

## 2026-10-12

| 时刻 | 事件 |
|---|---|
| 15:10 | tools/ 目录建立；JDK21/mill/make 本地化完成（无 sudo 环境） |
| 15:13 | 香山 clone @4b9ddb8 + 子模块 init 完成；mill 0.12.3 修复（用 mill-dist） |
| 15:14 | make verilog CONFIG=MinimalConfig 启动 |
| 15:21 | XSTop.fir（841MB）生成；firtool 转换启动 |
| 15:24 | RTL 生成完成：1579 文件、868MB、总耗时 10m03s |
| 15:26 | 阶段2 残留检查通过（门控感知版；无保护 DPI=0） |
| 15:27 | 工程创建 v1 失败（上游 xs_uart.tcl switch -regexp bug）→ 修复为 -exact |
| 15:29 | 工程创建 v2 成功；IP（ddr4:2.2 等）与 BD 全部生成；0 ERROR |
| 15:30 | 工程验证：part -1-e / NO_DIFF / CONFIG_RANK_WIDTH=1 / 3 个 XDC / top fpga_top_debug |
| 15:34 | bitstream 启动失败：make bitstream 重复创建工程 → 改用 tools/run_bitstream.sh（对现有工程） |
| 15:34 | **impl_1 正式 launch**（含 OOC IP 综合） |
| 15:35 | bin2fpgadata 就绪；riscv64 工具链本地化（7 deb 解包） |
| 15:36 | 自编译 hello world 验证：hello.bin→hello.txt 格式与加载器语义逐字节核对通过 |
| 15:46 | 主综合启动（synth_design -top fpga_top_debug，7 进程多线程） |
| 15:50 | **RTL elaboration 通过**（4m23s，峰值 12.2GB）——1579 文件 RTL+BD 例化无错 |
| 15:5x | 发现 fpga.xdc 行尾注释被 Tcl 误解析（LED/UART 约束+2 个 create_clock 未生效）→ 重写为注释独占整行（commit 6ba4f2d），impl 阶段将重新读取 |
| 16:11 | 综合映射阶段（RAM mapping，无错误） |
| 17:31 | **综合成功完成**: "Synthesis finished with 0 errors, 0 critical warnings and 1944 warnings"，耗时 1h45m（峰值虚拟内存 28.4GB） |
| 17:4x | **impl_1 启动**（opt→place→phys_opt→route→write_bitstream；约束重读正常） |
| 18:12 | **impl 失败**: opt_design DRC INBB-3 — 47 个 Difftest* 模块黑盒（difftest DPI 包装链，无保护定义） |
| 18:2x-18:4x | 排查: 空模块本身综合正常（隔离实验）、无重复定义、无宏泄漏、minimal wrapper 综合正常；确认 45 处 DummyDPICWrapper 例化（5 文件）是黑盒唯一来源 |
| 18:47 | 建立 17 分钟快速验证循环（`synth_design -rtl`：45 黑盒稳定复现） |
| 18:5x | **修复**: tools/patch_rtl.sh 新增步骤3 — 移除全部 45 处 DummyDPICWrapper 例化（difftest 关闭时纯悬空链），RTL 备份于 /tmp/rtl_backup |
| 19:0x | **修复验证通过**: 17分钟 elab 循环 → BLACKBOX CELLS: 0（修复前 45） |
| 19:09 | **重跑完整流程**（reset synth_1/impl_1 → launch impl_1 -to_step write_bitstream, jobs=8） |
| 21:33 | **第二轮综合成功**: 0 errors / 0 critical warnings / 1941 warnings（警告比首轮少3条=移除的difftest电路），耗时2h20m |
| 21:44 | **impl_1 启动**（约束重读：0 CRITICAL WARNING，fpga.xdc 修复生效） |
| 21:5x | **opt_design DRC 通过: 0 Errors**（首轮 47 个 INBB-3 黑盒错误已消除 ✓）|
| 22:10 | place_design 失败: DRC BIVC-1 — bank 20 电压冲突（DDR4 POD12 需 VCCO=1.2V vs clk7 LVDS 需 1.8V）|
| 22:2x | **修复**: fpga.xdc 中 clk7_p/n 改 **DIFF_SSTL12**（1.2V 兼容差分标准，PHINE HP20_GC02 DDR 参考时钟方案的等价电气标准；DIFF_SSTL12 不支持 DIFF_TERM_ADV，仅保留 DIFF_TERM）|
| 22:3x | 快速验证: read_xdc 后 report_drc BIVC-1 → **0 violations** |
| 22:3x | **重启 impl_1**（保留已完成的 synth_1, reset impl_1 → launch impl_1） |
| 23:0x | place_design 再次 DRC 失败: PORTPROP-6 — 综合 DCP 里烙着旧 DIFF_TERM_ADV=TERM_100（xdc 只能覆盖显式设置的属性; reset_property 不被 xdc 支持）|
| 23:2x | **修复**: fpga.xdc 用空值 `set_property DIFF_TERM_ADV {}` 清除残留 → read_xdc 后 PORTPROP-6 + BIVC-1 = 0 ✓（commit 6cf5ad4）|
| 23:3x | **第三次 impl 启动** |
| 00:0x | place DRC 通过(0 Errors)✓ 但 Phase 1.2 失败: Place 30-675 — JTAG_TCK(W13 普通IO)驱动 BUFG 违反时钟规则 |
| 00:1x | **修复**: TCK 移至 Y15(GPIO_5 = IO_L6P_HDGC_98 全局时钟脚), 其余四针循环调整; XDC 中采用无冲突移位顺序（旧 DCP 已烙引脚）; 5 分钟 opt 检查点直测: Phase 1.2 时钟放置通过 ✓（commit e723095）|
| 00:2x | **第四次 impl 启动** |
| 00:31 | **place_design 正式启动**: DRC 0 Errors ✓; Phase 1.2 IO/时钟放置完成(Checksum) ✓; 进入 Phase 1.3 —— 三大障碍全部清除，布局长跑开始 |

## 待续
- place (数小时) → phys_opt → route (数小时) → write_bitstream
- 预计完成: 2026-10-13 白天

## 2026-10-13

| 时刻 | 事件 |
|---|---|
| 02:25 | **place_design 成功完成** ✓ |
| 02:2x | **布局后时序: WNS=+0.779ns, TNS=0.000**（全部路径满足，-1 速度等级下余量健康）|
| 02:3x | 物理优化(phys_opt AggressiveFanoutOpt)完成, 进入布线(route Explore) |
| 04:34 | **route_design 成功完成** ✓ |
| 04:34 | **布线后时序完全收敛: WNS=+0.416 / TNS=0.000 / WHS=+0.010 / THS=0.000**（setup/hold 全部满足）|
| 04:3x | 布线后 DRC → write_bitstream 进行中 |
| **05:13** | **🎉 比特流生成完成** — `Bitgen Completed Successfully` / `write_bitstream completed successfully` / 布线后 DRC 0 Errors |
| 05:13 | 产物归档: `artifacts/fpga_top_debug.bit` (190MB, md5 43be3a59...) + `.ltx` + 时序/利用率/DRC 报告 |

## 最终指标（阶段4 通过）

| 指标 | 值 |
|---|---|
| 时序 (route 后) | **WNS=+0.416ns / TNS=0.000 / WHS=+0.010ns / THS=0.000**（全部收敛）|
| CLB LUTs | 1,538,143 / 4,085,760 = 37.65% |
| CLB Registers | 497,819 / 8,171,520 = 6.09% |
| Block RAM | 272 / 2,160 = 12.59% |
| URAM | 16 / 320 = 5.00%（`ram_style="ultra"` 提示生效）|
| Bonded IOB | 136 / 2,072 = 6.56% |
| 总耗时 | synth 2h20m + impl 6h40m ≈ 9 小时（含 3 次 DRC 问题修复迭代）|

## 阶段5/6 待办（需板卡）
> **完整实操指南见 [docs/onboard-runbook.md](onboard-runbook.md)**（硬件连接/烧录/串口/Hello/Linux/排障）

1. AEConsole 一次性配置（FMC2 VADJ=1.2V + 三路 PLL 时钟）→ docs/board-setup.md
2. AMD JTAG 线接 CN10 烧写 artifacts/fpga_top_debug.bit + .ltx（或 AEConsole 下载 .bin）
3. `make halt_soc` → `make write_ddr`(hello-xs.txt) → `make reset_cpu` → 串口 115200 看 Hello
4. Linux: `make write_ddr`(kmh-linux/data.txt) → `make reset_cpu` → root/bosc 登录

## 附加产物
- `artifacts/fpga_top_debug.bin` — AEConsole FPGA Bin Download 格式（SMAPx8，199MB，2026-10-13 生成）
- `docs/onboard-runbook.md` — 上板实操手册（连接/运行/排障全流程）
- `tools/patch_bootrom.{sh,tcl}` — ROM 启动补丁工具（见下）

## 上板调试记录（2026-10-14，真机）

| 事件 | 结论 |
|---|---|
| JTAG 频率 12MHz 报 44-49 | Digilent 线不支持该值 → 脚本已改为不设置频率（默认即合法） |
| write_ddr 报 27-2083 | 工程含 2 个 JTAG-AXI 核（32-bit flash / 64-bit DDR），脚本抓错核 → 已改为按 AXI_DATA_WIDTH=64 自动选择（真机验证 6 块 1 秒写完） |
| **串口无输出（根因）** | **bootrom COE 首条指令 `csrrci x0,0x744,8`（mnstatus 标准 Smrnmi 地址）在 4b9ddb8 上非法——该版本 mnstatus 位于 0x353（rocket-chip/CustomInstructions.scala）→ 复位即陷阱死循环**；且 6 字 COE 缺少 classic 布局中 0x88 转义入口的拷贝（ROM@0x88 全零）|
| 诊断证据 | DDR 镜像回读逐字正确 ✓；ROM 回读 == COE ✓（排除写入/时钟/序列问题）|
| **修复** | 通过 flash JTAG-AXI 向 ROM 的 0x00 与 0x88 双入口写入版本无关 3 指令启动（addiw/slli/jalr，无 CSR 操作），BRESP=OKAY，回读验证 ✓，reset_cpu 后 CPU 应正常启动 |
| 仓库根治 | `blk_mem_gen_0.tcl` 生成的 coe 已替换为兼容版（重综合后无需补丁）；现有比特流每次重新配置后运行 `bash tools/patch_bootrom.sh` 即可 |

## 上板调试记录 · 第二轮（2026-10-14 下午，串口始终静默）

| 步骤 | 发现 |
|---|---|
| ttyUSB 甄别 | ttyUSB1=FT232R（官方 Console 线适配器）✓；Digilent JTAG=FT232H 不占 tty 口；串口链路设备侧无误 |
| ROM 回读 | == COE ✓；DDR 回读 == hello 镜像 ✓（写入/时钟/时序全部排除）|
| 反汇编 COE | 首条 `csrrci x0,0x744,8`（mnstatus Smrnmi 地址）在 4b9ddb8（mnstatus=**0x353**）上非法 → 复位即陷阱；且 0x88 转义入口处无代码 → 已打运行时补丁（双入口 3 指令启动，readback 验证 ✓）|
| 补丁后仍然静默 | 排查复位架构：CPU 的 `io_reset` 实际接 **`cpu_rstn`**（core_def_xdma.sv:1731），该寄存器**只能由按键消抖 FSM 的 DONE 状态置位** |
| 消抖 FSM 分析 | 需要 `button_i = BG17引脚 && vio_sw5` 上完整的 1→0→1 序列；VIO 序列与物理按键均试过 |
| **DDR 魔法探针判定** | 向 DDR 写入双标记探针（M1@+0x10000=0xA5A5A5A5 / M2@+0x10004=0x5A5A5A5A）；VIO 复位+物理按键多轮后 **M1/M2 均未出现（0x10000 为未写过的随机内存）→ 判定 CPU 从未执行** |
| clk5 活性证据 | AXI_bridge（ROM/flash-JTAG-AXI）时钟全来自 clk5（SYS_INTER_CLK/ACLK=sys_clk_i）；ROM 可读 ⟹ **clk5 正常**（PLL GC04 已配置 ✓）|
| **结论** | 复位释放路径（物理按键引脚/消抖链路）在实机上不可靠；需 RTL+XDC 加固后重建 |

## v2 重建（2026-10-14 晚）

- **XDC**: `PULLUP` 加固 rstn_sw5/rstn_sw6（BG17/BG18）
- **RTL**: `fpga_top_debug.sv` — `cpu_rstn` 释放路径新增 **VIO sw5 上升沿直通**（`cpu_setn_rflag || (vio_sw5 && !vio_sw5_d)`），软件流程必然生效，不再依赖物理按键
- 工程重建使 `blk_mem_gen_0.tcl` 修复版 coe 生效（新比特流自带正确 bootrom，无需运行时补丁）
- 提交: `833b718`（RTL/XDC）；重建产物 → `artifacts/`（v2）

- 综合完成 → impl（place/route）→ write_bitstream
- 产物: `env-scripts/fpga_diff/fpga_vivado_kmh-nodiff/fpga_vivado_kmh-nodiff.runs/impl_1/fpga_top_debug.bit` + `.ltx`
- 日志: `logs/bitstream-run-*.log`

## 内存观察

物理 15GB + swap 63GB。综合峰值已达 28.3GB 虚拟内存（physical 用满、swap 兜底）；
实现阶段（place/route）预计更吃内存，若极端缓慢属预期现象。

## 上板调试记录 · 第三轮（2026-10-14 夜，v2 比特流 + 板卡诊断比特流）

### v2 比特流验证（纯 VIO 流程，不用板上任何按键）

| 步骤 | 结果 |
|---|---|
| 烧写 v2（md5 `30fa6d1e…`）| 成功；器件报告 2×JTAG-AXI + 1×MIG + 1×VIO ✓ |
| ROM 双入口回读（0x10000000 / 0x10000088）| == 修复版 3 指令启动码 ✓ **内置，无需运行时补丁** |
| 时序（post-route）| WNS +0.394 / TNS 0 / WHS +0.010 / THS 0 ✓ |
| 网表核对 | `cpu_rstn_reg`、`vio_sw5_d_reg`、消抖 FSM 均在综合网表 ✓ |
| NO_DIFF 时钟分支核实 | `inter_soc_clk = sys_clk_i`（无 DifftestClockGate 门控，该分支仅 `ifndef NO_DIFF` 编译）✓ |
| `make write_ddr` hello-xs | 6/6 块 100% 写入 ✓（64-bit 核自动选择生效）|
| `make reset_cpu`（纯 VIO）| 序列执行（sw4→sw5=0→sw6=0→sw6=1→sw5=1），但**串口仍静默** |
| 双探针判定 | ① DDR 魔法标记（M1/M2 @0x80010000）均未出现；② ROM 驻留探针（直接替换启动码，UART 优先打印）也无任何输出 → **CPU 仍未执行** |

### 板卡诊断比特流（tools/board_diag/，~20 分钟构建）

为区分"板卡外部条件"与"设计内部问题"，构建最小诊断设计：VIO(7 输入探针) + 四路时钟计数器
(clk5/6/7/8) + 复位链复刻（与 XiangShan 相同的 cpu_rstn 逻辑）+ VIO 控制的 115200 UART 发送器。

**上板地面真相（2026-10-14 22:05-22:07）**：

| 观测项 | 读出 | 结论 |
|---|---|---|
| cnt5 / cnt6 / cnt7 / cnt8 | 均在计数 | **四路时钟全部存活**（25M/200M/80M/1M）✓ |
| rstn_sw6 / rstn_sw5 / rstn_sw4 引脚 | 1 / 1 / 1 | 上拉有效，无按键干扰 ✓ |
| uart0_sin（适配器 TXD） | 1 | 串口适配器在线且空闲 ✓ |
| VIO→UART 发送 H/U/K | ttyUSB0 收到 **"HUK"** | **串口链路端到端工作**（FPGA 引脚→适配器→主机）✓ |
| 复刻复位序列（sw6=0→1→sw5=0→1）| cpu_rstn 0→0→0→**1** | **VIO 上升沿释放逻辑工作正常** ✓ |

→ **板卡外部条件（时钟/引脚/串口/复位逻辑）全部排除**；CPU 不执行的原因在 XiangShan 设计内部。

### v3 重建（2026-10-14 22:30 启动，纯 VIO + 全观测）

RTL 变更：
- `fpga_top_debug.sv`：复位**完全去引脚依赖**——`sys_rstn_to_core = vio_sw6`、`ddr_rstn_to_core = vio_sw4`、
  cpu_rstn 仅受 vio_sw6/vio_sw5 控制；三个复位引脚仅保留 DONT_TOUCH IBUF 作观测（满足"完全不用板卡按键/上下拉"要求）
- `core_def_xdma.sv`：新增诊断计数器输出
  - `dbg_cfg_ar_cnt / dbg_cfg_r_cnt / dbg_cfg_ar_addr`：CPU 外设口(→ROM/UART)取指 AR/R 活动
  - `dbg_ddr_ar_cnt / dbg_ddr_ar_addr`：CPU 访存口(→DDR) AR 活动
  - `dbg_uart_tx_edges`：UART TX 下降沿计数（CPU 是否写过串口）
  - `dbg_dm_state = {ndreset, halt, critical_error}`：调试模块是否扣住核心
- `kmh/SimTop_wrapper.sv`：驱动先前悬空的 `io_debug_reset / io_riscv_halt_0 / io_riscv_critical_error_0` 输出
- `vio_0.tcl`：0 → **7 输入探针**（32/32/32/36/31/16/16）；输出 sw4/5/6 保持（INIT=1）
- `fpga.xdc`：**RISC-V 调试 JTAG 空闲态加固**（TCK 下拉=无时钟、TMS 上拉=杂散边沿把 TAP 推向复位、
  TDI 上拉；TRSTn 原有上拉保持）——针对"CN43 排针悬空 → TAP 随机时钟化 → 随机 DMI 访问扣核"
  这一假设的预防性修复（上游用 LVCMOS18 专用引脚，我们用的是 GPIO 排针）

**判读表（v3 上板后）**：
| dbg_cfg_ar_cnt | dbg_ddr_ar_cnt | dbg_dm_state | 结论 |
|---|---|---|---|
| 0 | 0 | halt=1 | 调试模块扣住了核心 → 查链路/引脚 |
| 0 | 0 | 全 0 | 核心完全未发起总线 → 核心内部复位/时钟问题 |
| >0 | 0 | - | 取指在途但未完成（cfg/ROM 路径挂起）；看 last addr |
| >0 | >0 | - | CPU 已跑起来（跳到 DDR）；看 uart_tx_edges 与串口 |

- 构建日志: `logs/bitstream-run-20261014-2229.log`（预计 ~05:30 完成）
- 注意：**构建期间不可 kill**（kill+reset 会损坏 .jobs 运行状态，症状为 OOC 运行报
  `Project file does not exist... Provided: Vivado`）；如损坏，删除工程目录重新 `make project` 即可

## v3 上板结果 · 大突破（2026-10-15 凌晨）

v3 比特流（WNS +0.469，md5 `8767a0a6…`）烧写后探针第一读数即证明 **CPU 已运行**：

| 探针 | 读数 | 含义 |
|---|---|---|
| `dbg_cfg_ar_cnt=3`，末地址 `0x10000008` | CPU 从 ROM 取走 3 条启动指令（0x10000000/04/08）✓ |
| `dbg_ddr_ar_cnt` 持续增长，末地址 `0x8000xxxx` | CPU 已跳转 DDR 并持续取指 ✓ |
| `dbg_dm_state=0` | 无 ndreset/halt/critical_error ✓ |
| `dbg_uart_tx_edges` 增长 | CPU 写 UART，TX 线有真实波形 ✓ |

**修复点（v2→v3 的差异）**：复位完全去引脚依赖（纯 VIO）+ **CN43 调试 JTAG 引脚空闲态加固**
（TCK 下拉/TMS 上拉）。后者为主嫌疑：v2 中悬空 TCK/TMS 使 TAP 随机时钟化、随机 DMI 访问可扣核/触发 ndreset；
另外 v2 的 `sys_rstn_to_core = 引脚 && vio_sw6` 路径也在 v3 中彻底去除（完全满足"不用板卡按键/上下拉"要求）。

### 裸机 Hello 成功（2026-10-15 04:53）

```
X
Hello, XiangShan! MinimalConfig on PHINE PD-VU19P, 115200 8N1
```

工作负载：`sw/baremetal/hello2.S`（自设 UART 分频）。完整可用流程（纯 VIO/TCL）：

```bash
V=artifacts/v3
make halt_soc  FPGA_BIT_HOME=$V          # sw6=0 保持复位
make write_ddr WORKLOAD=sw/baremetal/hello2.txt
stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb -crtscts raw && cat /dev/ttyUSB0 &
make reset_cpu FPGA_BIT_HOME=$V          # sw6=1 + sw5 上升沿释放
```

### 三个必须记录的坑（全部踩过）

1. **UART 分频必须由程序设置**：本设计 uart16550 复位默认分频 ≈ 38400 波特（Xilinx IP 默认值按
   ACLK_FREQ_HZ 参数算，与实机 25MHz 不符）。Linux 内核会自行设置分频（所以官方 Linux 流程正常）；
   裸机程序必须自己写：`LCR=0x83 → DLL=14, DLM=0 → LCR=0x03`（25MHz/16/14 ≈ 111.6k ≈ 115200，误差 3%）。
   症状：115200 接收端收到全 0x00（Linux tty 把帧错误字节交付为 NUL）。
2. **官方 `ready-to-run/hello-xs.bin` 与本文档流程不兼容**：它向 `0x31B00000` 写 UART，而设计/官方文档
   （UART0_BASE）都是 `0x310B0000`（差一个半字节位置）→ 访问未映射地址 → **critical error → 核心停住**。
   自带工作负载请用 `sw/baremetal/` 下的自制版本。
3. **手写 `.txt` 工作负载必须用 `tools/bin/bin2fpgadata`**（或严格遵循 hw_axi 字节序约定：
   数据串 = 各 8 字节 beat 的 LE 十六进制表示，**从最高地址 beat 到最低地址排列**；即最右 = 最低地址）。
   字节序写反的程序会在 DDR 里变成乱码 → 执行即 critical error。核验方法：写后用
   `hw_axi_2` 回读 DDR[0] 与目标首字比对。

### v3 会话中另两个观察

- 计数器为**上电后单调累计**（不被 reset_cpu 清零）；对照"FPGA 重新配置"才能清零，
  上板诊断时注意基线。
- CPU 在 `j .` 自旋时 `dbg_*` 计数完全静止、`dm_state=0`——这是"健康停车"的特征；
  若 `dm_state=1`（critical_error）且计数静止 = 程序执行出错（陷阱/访存错误）。

## 官方 Linux 镜像 · 最终结论与修复（2026-10-15 11:20 完成）

### ✅ 最终结果：完整启动到可交互 shell（115200 8N1）

```
[    1.019208] 310b0000.serial: ttyS0 at MMIO 0x310b0000 (irq = 0, base_baud = 1562500) is a 16550A
[    1.030475] printk: legacy console [ttyS0] enabled
[    3.226277] Run /init as init process
Starting syslogd: OK / Starting klogd: OK
Linux buildroot 6.10.7-00060-g80dff865005d-dirty #25 SMP riscv64 GNU/Linux
# uname -a
Linux buildroot 6.10.7-... riscv64 GNU/Linux
# ls /
bin init linuxrc opt run tmp  dev lib media proc sbin usr  etc lib64 mnt root sys var
```
证据: `artifacts/v3/evidence/serial_linux_full_boot_success.log`（完整启动日志）
     `artifacts/v3/evidence/serial_linux_shell_interactive.log`（shell 命令交互）

### 根因：镜像 DTB 的 UART 时钟假设与本设计不符

- 官方镜像内嵌 DTB（OpenSBI embedded，镜像偏移 0x1e000）中
  `serial@310b0000` 的 **clock-frequency = 50 MHz**；
- 本设计（env-scripts fpga_diff + PHINE 板）AXI_bridge/JTAG-DDR 子系统的实际时钟
  为 **clk5 = 25 MHz**（用户 AEConsole PLL GC04 配置，与官方约束文件一致）；
- 软件按 50MHz 算分频 → 实际波特率只有标称的一半：
  OpenSBI 横幅落到 57.6k（之前只在 57600 下可读），内核控制台分频错乱（日志读不出）。

### 修复（纯软件，无需重建比特流！）

```bash
python3 tools/patch_linux_dtb_clock.py sw/linux/data.txt sw/linux/data_patched.txt
# 把镜像内嵌 DTB 的 clock-frequency 从 50MHz 改为 25MHz（唯一一处）
```
修复后 **OpenSBI、Linux 内核、自制裸机三者全部统一为 111.6 kbps（分频=14，25MHz/16/14）
≈ 115200 8N1**，串口终端按 115200 打开即可看到全部输出。
（内核日志中 `base_baud = 1562500` = 25MHz/16，正是分频 14 的来源，与裸机完全一致。）

### 复现命令（纯 VIO/TCL，全程不用板卡按键）

```bash
cd <本仓库根目录>                     # ReConstruct（一键版本: flows/07_run_linux.sh）
source tools/env.sh
V=$PWD/build                          # 比特流/LTX 所在目录

# ① 生成打过补丁的 Linux 镜像（只需一次; 未下载过先跑 sw/linux/fetch_image.sh）
python3 tools/patch_linux_dtb_clock.py sw/linux/data.txt sw/linux/data_patched.txt

# ② 暂停 CPU
make -C third_party/env-scripts/fpga_diff halt_soc FPGA_BIT_HOME=$V

# ③ 写入镜像（13.3MB，约 55 秒；串口 115200 打开并开始抓包）
make -C third_party/env-scripts/fpga_diff write_ddr WORKLOAD=$PWD/sw/linux/data_patched.txt
stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb -crtscts raw && cat /dev/ttyUSB0 &

# ④ 释放 CPU → OpenSBI → Linux 内核 → buildroot shell（约 4 秒）
make -C third_party/env-scripts/fpga_diff reset_cpu FPGA_BIT_HOME=$V
```

### 调试过程中澄清的两个"红鲱鱼"（供后续 debugging 参考）

1. **JTAG 看到 DDR 与 CPU 视角不一致**：CPU 经缓存写入的数据（如 OpenSBI 运行期生成的
   DTB）在未全部写回前，JTAG(hw_axi_2) 读到的是 DDR 的滞后内容（曾表现为 DTB 里
   几处 8 字节零 hole、"clock-frequency" 字符串被截断成 "cloc"）。
   **CPU/内核经缓存读到的数据始终是正确的**（用 wtest4 复制 DTB 到另址验证过），
   因此这些"损坏"不影响内核。诊断时凡比较 CPU 写入的内存，务必以 CPU 侧视图为准。
2. **写路径本身是健康的**：wtest3 测试（CPU 填 256KB、cbo.flush 写回、48MB 大填充驱逐后
   再读）零错误；数据最终都会正确落盘，只是写回时机不可控。

### 裸机（自制工作负载）的三个坑（已全部踩过并归档）

1. **C 程序入口必须先设置栈**：ROM 跳转过来时 sp 是垃圾值，C 函数序言
   `sd ra,8(sp)` 会写非法地址 → 存储异常 → mtvec=0 → 双重陷阱 → critical error。
   正确做法：用汇编启动桩（见 `sw/baremetal/start.S`）：`.text.start` 段放
   `li sp, 0x81000000; j bmain`，链接脚本 `ENTRY(_start)` + `*(.text.start)` 放最前。
2. **目标文件别带 build-id**：`.note.gnu.build-id` 会占 0x80000000-0x23，把代码顶到
   0x24 → ROM 跳到 0x80000000 执行的就不是入口。加 `-Wl,--build-id=none`
   （并在链接脚本 `/DISCARD/` 里丢弃 .note/.eh_frame）。
3. **UART 分频必须程序自设**：16550 复位默认分频 = 162 → **9600 波特**
   （实测 LLC/DLM 读出 0xA2）。程序必须写
   `LCR=0x83 → DLL=14, DLM=0 → LCR=0x03`（25MHz/16/14 ≈ 111.6k ≈ 115200）。

以上三个修复后的模板见 `sw/baremetal/`（hello.c/hello2.S/wtest3.c/start.S/wtest.lds）。

### 硬件侧可选后续（非必需）

当前方案不依赖任何硬件改动。若想进一步"精确"115200（消除 3% 分频取整误差），
可让 AXI_bridge/JTAG-DDR 子系统的 ACLK 改接 50MHz（BD 内含跨时钟单元，CPU 仍 25MHz），
此时不补丁镜像即可，但需重新综合实现（~7h），收益极小，不建议。

## VIO 脚本 44-152 报错修复（2026-10-15 晚，用户反馈）

**现象**：`make halt_soc` / `make reset_cpu` 报
```
ERROR: [Labtoolstcl 44-152] hw_probe '<const0>' does not have property 'OUTPUT_VALUE'
```

**根因**：Vivado 的 `get_hw_probes <模式>` 查询有一个回退行为——当该 VIO 的探针
**刚被 `set_property` 修改过**（值发生变化、尚未 commit）时，模式匹配会失效并
**回退返回该 VIO 的全部探针**（列表以 `<const0>` 开头；它们是 v3 中 VIO 输入探针
打包进去的 3 个零位常量）。旧脚本写法：
```tcl
set_property OUTPUT_VALUE 0 [get_hw_probes vio_sw6 ...]
commit_hw_vio [get_hw_probes {vio_sw6} ...]     ;# ← 此处回退成全量探针
```
于是 `commit_hw_vio` 拿到混合列表，在第一个 `<const0>` 上报错中止。
写入值恰好"没有变化"（如 sw6 已是 0 再写 0）时不触发——这解释了此前
为什么偶尔成功、偶尔失败。

**修复**：所有脚本改为 **"先按精确名解析出输出探针对象（校验其具备
OUTPUT_VALUE 属性），之后全程复用同一对象"**，修改后不再做模式查询：

- `env-scripts/fpga_diff/vivado/scripts/halt_soc.tcl`
- `env-scripts/fpga_diff/vivado/scripts/reset_cpu.tcl`（含 `ddr_rstn_to_core` 兼容回退）
- `env-scripts/fpga_diff/vivado/scripts/reset_ddr.tcl`
- `tools/verify_v3.tcl`、`tools/board_diag/scripts/run_diag.tcl`

解析结果会打印 `resolved: sw4=... sw5=... sw6=...` 便于核对。

**验证**：在复现场景（sw6 由 1 写 0，即"值变化"路径）下 halt_soc/reset_cpu 均正常；
完整流程 `halt_soc → write_ddr(13.3MB) → reset_cpu → Linux 启动到 shell` 通过。
