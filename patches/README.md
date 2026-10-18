# 补丁系列总览

本目录按**目标上游**分三组：

| 目录 | 目标 | 内容 |
|---|---|---|
| `env-scripts/` | `OpenXiangShan/env-scripts` | 板卡移植 12 个补丁（本文下半部分）|
| `xiangshan-xvbbp/` | `OpenXiangShan/XiangShan` | XVBBP 7 条自定义指令的核心侧支持（5 个 Scala 文件）|
| `nexus-am/` | `GaryCAICHI/nexus-am` | FFT/MIMO 板级适配（UART 25MHz、mstatus.VS、lp64d ABI）|

`flows/02_clone_deps.sh` 自动拉取对应上游版本并应用全部补丁；各系列的来源与再生命令
见各自目录的 `README.md`。

---

# env-scripts 移植补丁系列

本目录的 12 个补丁是**本工程对上游 `OpenXiangShan/env-scripts`（fpga_diff 流程）的完整移植改动**：
把该流程适配到 PHINE PD-VU19P-2-LSI SOLO v3 板（器件/引脚/时钟/DDR/复位链路）并修复实机问题。

## 应用方式（flows/02_clone_deps.sh 自动完成）

```bash
git clone https://github.com/OpenXiangShan/env-scripts.git third_party/env-scripts
cd third_party/env-scripts
git checkout 876f39e4cf690745a3951ac6c06ce5475d7d4f16          # ★ 补丁基线（勿跳过）
git -c user.name=ReConstruct -c user.email=reconstruct@local \
    am /path/to/ReConstruct/patches/env-scripts/*.patch
```

- **基线 commit**：`876f39e4cf690745a3951ac6c06ce5475d7d4f16`（上游 main，2026-09-24）
- 补丁为线性历史（`git am` 顺序应用）；**全部是对上游已有文件的修改，无新增/删除文件**，
  因此不存在路径冲突风险
- `git am` 需要 committer 身份；脚本用 `-c user.name/email` 兜底（上一行写法），
  避免干净克隆环境无 git 身份时报错中止
- 校验：应用后 `third_party/env-scripts/fpga_diff` 应与本工程开发分支逐字节一致
  （本仓库构建时已用 `diff -r` 验证通过）

## 补丁清单（按应用顺序）

| # | 主题 | 解决的问题 |
|---|---|---|
| 0001 | **移植 fpga_diff 到 PHINE PD-VU19P** | 器件改 `xcvu19p-fsva3824-1-e`；重写 `fpga.xdc`（板卡 IO：UART/LED/按键/JTAG/时钟引脚）与 `ddr.xdc`（FMC2 DDR4 116 引脚）；顶层 `fpga_top_debug.sv` 裁剪（去掉 PHINE 无对应硬件的 SD/以太网口等）；BD/IP tcl 相应调整 |
| 0002 | 修 `xs_uart.tcl` 参数解析 | `switch -regexp` 下 `--cpu_files` 会先匹配 `--cpu` 造成参数串扰 → 改 `switch -exact` |
| 0003 | 修 `fpga.xdc` 行尾注释 | Tcl 把行尾 `#` 之后当参数 → LED/UART 的 PACKAGE_PIN 与 2 个 create_clock 被静默丢弃；注释改为独占行 |
| 0004 | 修 bank 20 IO 电压冲突（BIVC-1）| DDR4 的 DIFF_POD12（VCCO=1.2V）与 clk7 的 LVDS 冲突 → clk7 改 `DIFF_SSTL12`，并清综合 DCP 中残留的 `DIFF_TERM_ADV` |
| 0005 | JTAG_TCK 移到 Y15 | 原引脚走 IO→BUFG 违反时钟规则（Place 30-675）→ 改用全局时钟引脚 `Y15 (GPIO_5, IO_L6P_HDGC)`，并给出无冲突的 LOC 写入顺序 |
| 0006 | JTAG 频率设置加固 | Digilent 线不支持硬编码 12MHz（Labtoolstcl 44-49）|
| 0007 | **完全去掉 JTAG 频率设置** | 默认值即合法（6 的前置尝试后定稿）|
| 0008 | `jtag_write_ddr.tcl` 自动选核 | 设计含两个 JTAG-AXI 核（32-bit flash / **64-bit DDR**），原脚本写死 32-bit 核 → 按 `AXI_DATA_WIDTH==64` 自动选择（Labtools 27-2083 的根因）|
| 0009 | **修 bootrom coe** | 上游 coe 首条指令 `csrrci x0,0x744,8`（mnstatus Smrnmi 标准地址）在香山 commit 4b9ddb8 上非法（该版本 mnstatus 位于 `0x353`）→ 复位即陷阱、CPU 无任何输出；改为版本无关的 3 指令启动（`addiw/slli/jalr`，无 CSR 操作），并在 0x88 转义入口复制 |
| 0010 | CPU 复位释放加固（v2）| 物理按键消抖链路在实机上不可靠 → `rstn_sw5/sw6` 加 PULLUP；`cpu_rstn` 释放增加 **VIO sw5 上升沿直通** |
| 0011 | **纯 VIO 复位 + 上板可观测性（v3）** | 复位完全去引脚依赖（`sys_rstn_to_core=vio_sw6`、`ddr_rstn_to_core=vio_sw4`、`cpu_rstn` 只受 VIO 控制）；新增诊断计数器（CPU 取指/访存 AR 计数、UART TX 边沿、`{ndreset,halt,critical_error}`）；VIO 增 7 个输入探针；CN43 调试 JTAG 引脚空闲态加固（TCK 下拉/TMS/TDI 上拉，防止悬空引脚随机时钟化 TAP 造成随机 DMI 访问）|
| 0012 | 复位脚本探针解析加固 | Vivado `get_hw_probes <模式>` 在探针刚被修改后会**回退返回该 VIO 全部探针**（含 `<const0>`），`commit_hw_vio` 报 44-152 中止 → 改为"先按精确名解析输出探针对象、之后复用同一对象" |

## 上游对照与许可

- 上游仓库：<https://github.com/OpenXiangShan/env-scripts>（Apache-2.0）
- 本补丁系列仅修改 `fpga_diff/` 下的 22 个文件；其余子项目未改动
- 详细调试过程与根因分析见 [`docs/build-log.md`](../docs/build-log.md)
