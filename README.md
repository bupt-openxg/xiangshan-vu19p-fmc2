# ReConstruct — 香山昆明湖（MinimalConfig）部署到 PHINE PD-VU19P-2-LSI SOLO v3

把香山昆明湖处理器（MinimalConfig 单核）**从零复现**到 PHINE PD-VU19P 开发板：
生成 RTL → Vivado 2024.2 综合/实现出比特流 → 上板运行**裸机 Hello World**（串口输出）
与**官方 Linux 内存镜像**（OpenSBI → 内核 → buildroot shell）。

**全部板卡操作经 JTAG + VIO/TCL 完成，不使用板上任何按键/开关，不依赖引脚上下拉。**

- 依赖（香山源码 / env-scripts）由脚本克隆到 `third_party/` 并应用补丁系列，**不使用 git submodule**
- 环境（JDK/mill/riscv 工具链等）全部本地化到本仓库 `tools/`，**不需要 sudo**
- 上板全流程经 `flows/` 一键脚本驱动，串口统一 **115200 8N1**

> **分支**：`kmh-v2`（本 README 主体：香山 2025-02 基线，含裸机 + Linux）；
> `kmh-v3`（V3 升级，暂缓）；`xvbbp`（上游香山 2026-01 基线 + XVBBP 7 条自定义向量指令
> + FFT/MIMO 裸机负载，**只做裸机、不涉及 Linux**，见 [docs/xvbbp.md](docs/xvbbp.md)）。

---

## 一、前置要求

| 类别 | 要求 |
|---|---|
| 硬件 | PHINE PD-VU19P-2-LSI **SOLO v3** 板；FMC2 槽 + UV-FMCH-PDDR4DME 子卡 + 1Rx8 (单 rank) DDR4 SODIMM；AMD/JTAG 下载线（接 **CN10**）；串口适配器（FT232R，接后面板 **User RJ45** 口）|
| 板卡配置 | AEConsole 配 **VADJ = 1.2V**；PLL 输出 **GC02=80MHz**（DDR 参考）、**GC04=25MHz**（CPU 系统时钟）、**GC06=1MHz**（RTC）——见 [docs/board-setup.md](docs/board-setup.md) |
| 软件 | Linux x86_64；**Vivado 2024.2**（含 license）；`git curl python3 unzip dpkg-deb`；磁盘 **≥80GB**；**swap ≥32GB**（综合峰值内存 ~30GB）|
| 系统依赖 | Ubuntu 24.04(noble) 推荐（工具链 deb 与版本对齐）；其他发行版亦可（交叉工具链取本发行版版本）|

## 二、快速开始（三阶段）

### A. 装环境 + 拉依赖（首次，约 30 分钟，下载 ~1.2GB）

```bash
cd <本仓库>                      # 克隆得到, 例如 ~/ReConstruct
flows/00_host_check.sh           # 前置检查(只读; JTAG/串口为软检查, 板卡未连接不影响构建)
flows/01_setup_toolchain.sh      # JDK21(GraalVM)/mill/make/riscv64 交叉工具链/bin2fpgadata
flows/02_clone_deps.sh           # clone XiangShan@4b9ddb8 + env-scripts(main) + 应用 12 个移植补丁
```

### B. 构建比特流（约 7.5 小时）

```bash
flows/03_gen_rtl.sh              # make verilog(MinimalConfig) + 模拟器残留检查 + RTL 后处理 (~30 分钟)
flows/04_build_bitstream.sh      # 建工程 + 综合 + 实现 + 比特流 → build/fpga_top_debug.bit (~7 小时)
```

> 想跳过 7 小时构建：把已获得的 `fpga_top_debug.bit` + `.ltx` 放入 `build/` 即可直接进入 C 阶段
> （作者可将产物挂在 GitHub Releases，供他人下载）。

### C. 上板运行（板卡就绪后，约 5 分钟）

```bash
flows/05_program_fpga.sh         # 烧录比特流 (JTAG)
flows/06_run_hello.sh            # 裸机 Hello：串口 115200 输出 "Hello, XiangShan! ..."
flows/07_run_linux.sh            # 官方 Linux 镜像（自动下载+时钟修正）→ buildroot shell
```

### D. XVBBP 分支（仅 xvbbp；裸机 + FFT/MIMO）

```bash
flows/02_clone_deps.sh           # 香山@0a150819 + 5 文件补丁 + yunsuan@b4a62448 + nexus-am
flows/03_gen_rtl.sh && flows/04_build_bitstream.sh
flows/05_program_fpga.sh && flows/06_run_hello.sh
flows/09_build_xvbbp_sw.sh       # 官方 FFT/MIMO 代表集 → build/xvbbp/
flows/10_run_xvbbp.sh smoke      # 七指令冒烟：期望 "EXT instruction demo: PASS"
flows/10_run_xvbbp.sh mimo-ext-zf4
```

- 该分支**不包含 Linux**；核心侧只加了 5 个文件（见 `patches/xiangshan-xvbbp/`），
  运算数据通路在 `GaryCAICHI/YunSuan@b4a62448` 这个 Scala 包里（flows/02 固定 SHA）。
- 软件必须用 xPack GCC 15（`tools/setup/08`，flows/01 自动安装）；AM 板级补丁见
  `patches/nexus-am/`（UART 25MHz / mstatus.VS / lp64d）。
- 8-wide 后期线路综合工作集约 90–110GB：**15GB 机器预计只能 swap 抖动**，建议构建机
  RAM ≥64GB（128GB 推荐）。

（可选）板卡诊断：`flows/08_board_diag.sh` —— 独立小工程（~20 分钟），
分钟级验证四路时钟/复位引脚电平/复位逻辑/串口链路，排障首选（见 [docs/board-diag.md](docs/board-diag.md)）。

## 三、目录结构

```
ReConstruct/
├── flows/                 ★ 编号流程脚本 00..10（幂等, 依序执行即完成全流程）
├── patches/               ★ 三个补丁系列（各自的 README 记录来源/基线/再生方法）
│     ├── env-scripts/      板卡移植 12 个补丁
│     ├── xiangshan-xvbbp/  XVBBP 核心 5 文件补丁（xvbbp 分支）
│     └── nexus-am/         FFT/MIMO 板级 3 个补丁（xvbbp 分支）
├── third_party/           依赖克隆（flows/02 拉取；gitignored）
│     ├── XiangShan/        香山源码（kmh-v2: @4b9ddb8；xvbbp: 0a150819+补丁+yunsuan@b4a62448）
│     ├── env-scripts/      上游 main + 本仓库补丁（= 移植后的 FPGA 工程）
│     ├── nexus-am/         (xvbbp) AM fork + 板级补丁
│     └── riscv-xvbbp/      (xvbbp) 冻结的 FFT/MIMO 软件包
├── hw/                    硬件：pinmaps/(引脚表) · diag/(板卡诊断设计) · README
├── sw/                    软件负载：baremetal/(裸机源码+Makefile) · linux/(镜像获取与修正) · xvbbp/(负载说明)
├── tools/                 工具箱：env.sh(环境入口) · setup/(00..08 工具链安装)
│                          · run_hello.sh/run_linux.sh/prog_bitstream.tcl/read_probes.tcl
│                          · patch_rtl.sh/patch_linux_dtb_clock.py/gen_ddr_xdc.py · checks/
├── docs/                  文档：run-guide · onboard-runbook · build-log · xvbbp · board-setup
│                          · board-diag · workload-format · rtl-check-report · evidence/(实测证据)
├── build/                 (gitignored) 构建产物：fpga_top_debug.bit/.ltx、日志、xvbbp/ 负载
└── logs/                  (gitignored) 比特流构建日志
```

## 四、已实测状态（重要）

### xvbbp 分支（本轮；裸机 + FFT/MIMO）

| 环节 | 状态 |
|---|---|
| 解耦与核心补丁 | ✅ 核心侧增量仅 5 个 Scala 文件（+36/−3）；补丁由 `0a150819..e34d8adb` 精确生成，无无关改动；V 数据通路固定在 `GaryCAICHI/YunSuan@b4a62448` |
| 依赖集成（flows/02）| ✅ 上游香山 @`0a150819` 浅拉 + 11 顶层/3 嵌套 submodule + yunsuan 包固定 + 本地提交（工作树干净）+ nexus-am/riscv-xvbbp/env-scripts 全部就绪 |
| RTL 生成（flows/03）| ✅ 1821 文件；模拟器残留检查通过；ClockGate 透传；生成 RTL 含 `io_isComp/io_isConj`（VIMac64bStage1/VIMacU），确认扩展逻辑进入网表 |
| FFT/MIMO 离线构建（flows/09）| ✅ 官方 7 个代表镜像构建成功（smoke / FFT ext·scalar / MIMO ext·scalar）；工具链 xPack GCC 15 + 3 个 AM 板级补丁 |
| 本机综合/比特流（flows/04）| ✅ **综合成功**：2h06m，0 errors / 0 critical warnings，峰值 31.9GB；⚠️ 本机实现布线已完成（WNS **+0.709ns**、WHS +0.010ns、0 路由错误），但 00:40 被 **systemd-oomd** 杀掉整个会话，`routed.dcp`/比特流未落盘 → 已停止本机实现，转新构建机（见 `docs/xvbbp.md` 的中断记录与恢复手段）|
| 上板（flows/05/06/10）| ⏳ 待比特流 |

### kmh-v2 分支（2026-10-17 全链实测）

| 环节 | 状态 |
|---|---|
| 依赖克隆 + 补丁链（flows/02）| ✅ **在本仓库内实测**：XiangShan@`4b9ddb8` 浅拉取 + env-scripts 基线 `876f39e` → `git am` 12 补丁 → 与开发分支逐文件一致；submodule（顶层 10 个 + 嵌套 3 个）全部对齐父仓库记录的 SHA。实测修复了两类会让后续编译报 `not found: object xxx` 的陷阱：① `--depth 1` 停在默认分支顶端；② 默认分支顶端恰等于记录 SHA 时工作树为空（fudian/aia）。另：只初始化构建必需的嵌套模块，依赖体积 156MB（上游递归全拉为 1.7GB）|
| 工具链安装（flows/01）| ✅ `setup/00`（make 本地化）与 `setup/03-07` 已实测通过（03：已有 swap 自动跳过；04/05：厂商 BSP 未放置时优雅退出；06：bin2fpgadata；07：riscv64 交叉工具链 + make + xlrd）。`setup/01-02`（GraalVM JDK 21 / mill，约 1.2GB 下载）本轮网络受限未复跑，URL 与校验逻辑保持原样 |
| RTL 生成（flows/03）| ✅ **在本仓库内实测通过**：`make verilog` 产出 1579 文件，模拟器残留检查通过（DPI 全部 `ifndef SYNTHESIS` 门控），后处理完成。与原工程**上板验证过的同一份 RTL 逐字节比对：1578/1579 一致**；唯一差异 `XSTop.fir` 是 FIRRTL 中间文件的源码定位器绝对路径字符串（345 万处，不参与综合），综合输入（`XSTop.sv` 等）全部一致 |
| 裸机负载（sw/baremetal）| ✅ **在本仓库内实测**：`make` 产出与上板实测原产物逐字节一致（`hello.elf/bin/txt` 等 9 个产物中 7 个完全相同；2 个 `.elf` 仅差 gcc 随机临时目标文件名 `ccXXXXXX.o`，`.bin/.txt` 均逐字节相同）|
| 比特流构建（flows/04）| ✅ **在本仓库内实测完成**（2026-10-17）：综合 1h51m（0 errors / 0 critical warnings），布线 WNS **+0.469ns**（与原构建完全一致）、DRC 0 Errors；产出 `fpga_top_debug.bit`（199MB）+ `.ltx`。中途经历一次主机崩溃，用 `reset_runs -prev_step` 从 phys_opt 检查点续跑（仅损失 route 起始约 5 分钟）|
| 烧录 + 裸机 Hello（flows/05/06）| ✅ **在本仓库内实测**（2026-10-17）：`End of startup status: HIGH`；串口 115200 输出 `Hello, XiangShan! (MinimalConfig on PHINE PD-VU19P, 115200 8N1)`（实录 `docs/evidence/serial_hello_freshbuild.log`）|
| 官方 Linux 镜像（flows/07）| ✅ **在本仓库内实测**（2026-10-17）：OpenSBI v1.5.1 → Linux 6.10.7 → **buildroot shell 可交互**（`uname -a` / `ls /` 实录 `docs/evidence/serial_linux_freshbuild_*.log`；启动日志与原成功证据逐字节一致，仅内核时间戳微秒抖动）。失败变体（串口 echo 自环回）留档 `serial_linux_echo_bug_repro.log` 供排障对照 |
| 板卡诊断（flows/08）| ✅ 原始工程实测（四路时钟/引脚/复位逻辑/串口链路全部通过）；本轮未重跑 |

> 上板实测中发现并修复的交付缺陷（均已入库）：`setup/02-mill.sh`（全新环境下缺目录/launcher，curl 直接失败）、`prog_bitstream.tcl`/`read_probes.tcl`（Tcl 误用 bash 的 `${VAR:-默认}` 展开）、`flows/05`（未检查烧录成功标记）、`run_hello.sh`/`run_linux.sh`（`stty raw` 不会关闭 echo → 主机把 FPGA 输出回灌成输入，Linux 表现为 `ttyS0: input overrun` 与 init/shell 反复被打断）。

实测样例输出见 `docs/evidence/`（串口日志、DTB 证据等）；完整调试与根因见 `docs/build-log.md`。

## 五、板卡就绪后的验证清单

```bash
flows/05_program_fpga.sh     # 期望: End of startup status: HIGH
flows/06_run_hello.sh        # 期望串口: Hello, XiangShan! (MinimalConfig on PHINE PD-VU19P, 115200 8N1)
flows/07_run_linux.sh        # 期望: OpenSBI 横幅 → Linux 日志 → "#" 提示符 (buildroot shell)
# xvbbp 分支（无 Linux）：
flows/10_run_xvbbp.sh smoke          # 期望: EXT instruction demo: PASS + CHECK: PASS
flows/10_run_xvbbp.sh fft-ext-1024   # 期望: EXT demo PASS + CHECK: PASS（scalar 后端只要求 CHECK: PASS）
flows/10_run_xvbbp.sh mimo-ext-zf4   # 期望: MIMO_VALIDATE_PASS + MIMO_DONE
```
- 串口参数：**115200 8N1**；设备为 FT232R（`/dev/ttyUSB*`，用
  `udevadm info -q property -n <dev> | grep ID_MODEL` 甄别）
- Hello 与 Linux 可反复切换（无需重烧比特流）；Linux 镜像写入约 55 秒
- 排障顺序：`flows/08_board_diag.sh`（板卡外部条件）→ `tools/read_probes.tcl`（读全部 VIO 探针：
  CPU 取指/访存计数、复位链、调试模块状态）→ 对照 `docs/evidence/` 样例日志

## 六、常见问题（摘要，详见 [docs/run-guide.md](docs/run-guide.md) §5）

| 现象 | 处理 |
|---|---|
| 串口无输出 | 终端须为 115200 8N1；确认设备号；确认已执行 `reset_cpu`（脚本自动）|
| Linux 只出 OpenSBI 横幅 | 未使用时钟修正版镜像（`flows/07_run_linux.sh` 会自动生成，勿手工用 `data.txt`）|
| `make write_ddr` 报 27-2083 | 脚本已自动选择 64-bit DDR 的 JTAG-AXI 核（补丁 0008）|
| JTAG 被占用 | 同一时刻仅允许一个 Vivado 会话占用 JTAG |
| 构建中断 | 用 `tools/rerun_bitstream.sh` 重置运行后重跑；**不要删除工程目录**（见 build-log 44-152/Provided:Vivado 两例）|

## 七、第三方组件与归属

本仓库**不分发**以下内容（由脚本获取或需自行准备）：

- **香山 XiangShan**（`third_party/XiangShan`，脚本克隆；kmh-v2 @`4b9ddb8`，xvbbp @`0a150819`+补丁）—— 遵循其原始许可
- **OpenXiangShan/env-scripts**（`third_party/env-scripts`，脚本克隆 + 本仓库 `patches/` 补丁）—— Apache-2.0
- **GaryCAICHI/YunSuan**（xvbbp 的 V 数据通路包，@`b4a62448`）、**GaryCAICHI/nexus-am**（@`9dc5f1e1`）与
  **bupt-openxg-lab/riscv-xvbbp**（@`c553928`，FFT/MIMO 软件包）—— 各源文件保留原许可；
  XVBBP 设计与实现归 Cai Chi 等，见 `docs/xvbbp.md` §7
- **官方 Linux 内存镜像**（`sw/linux/fetch_image.sh` 从 [XiangShan-doc 资源目录](https://github.com/OpenXiangShan/XiangShan-doc/tree/main/docs/integration/resources) 下载，sha256 固定）
- **PHINE 厂商 BSP**（AEConsole / 原理图 / 引脚表，需向板卡厂商获取；本仓库仅保留派生引脚表 `hw/pinmaps/`）
- 本仓库自身内容（`flows/`、`tools/`、`docs/`、`hw/diag/`、`sw/baremetal/` 等）**暂未附加 License 文件**，作者将另行决定。
