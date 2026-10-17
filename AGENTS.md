# AGENTS.md — 代码智能体在本仓库的工作指南

> 本文件会被 ZCode / 兼容 Agent 自动加载。改仓库结构或流程时请同步更新它。
> 人类用户文档入口：[README.md](README.md)；完整调试史：[docs/build-log.md](docs/build-log.md)。

## 这是什么

**ReConstruct**：把香山昆明湖（Kunminghu，MinimalConfig 单核）从零复现到
**PHINE PD-VU19P-2-LSI SOLO v3** 开发板 —— 生成 RTL → Vivado 2024.2 出比特流 →
上板运行**裸机 Hello World** 与**官方 Linux 内存镜像**（OpenSBI → 内核 → buildroot shell）。
**全部板卡操作经 JTAG + VIO/TCL，不使用板上任何按键/开关。**

## 黄金规则（违反会白白浪费数小时）

1. **不要碰板上按键/拨码**（设计里它们被路由为 VIO 探针，物理按键无意义且板卡无上下拉）。
   一切操作走 `third_party/env-scripts/fpga_diff` 的 `make halt_soc / write_ddr / reset_cpu`
   或 `flows/` 一键脚本。
2. **串口一律 115200 8N1，且 `stty` 必须显式 `-echo`**（GNU `stty raw` 不会关 echo；
   否则主机行规程把 FPGA 输出回写给 FPGA，Linux 表现为 `ttyS0: input overrun`、
   init/shell 反复被打断）。运行 Linux 必须用时钟修正版镜像（`flows/07` 自动生成；
   手工跑时**勿**直接把 `sw/linux/data.txt` 灌进 DDR）。
3. **不要删除 Vivado 工程目录**（`third_party/env-scripts/fpga_diff/fpga_vivado_kmh-nodiff/`）；
   构建中断后用 `tools/rerun_bitstream.sh` 重置重跑。
4. **JTAG 同一时刻只允许一个 Vivado 会话**。
5. **`third_party/` 与 `build/` 不入库**；改动上游行为必须经 `patches/env-scripts/`
   补丁系列（新补丁 = `git format-patch` 落盘 + 更新 `patches/README.md`），
   只在 `third_party/` 里的改动会在下次 `flows/02` 被覆盖。
6. **生成 RTL 前保证 `third_party/XiangShan` 树干净（含全部 submodule）**，
   否则 RTL 会带 `gitDirty=1` 标记（香山把 git 状态嵌进生成代码）。
7. **强制重生成 RTL 要先把产物删掉**：`rm -rf third_party/XiangShan/build/rtl`
   （make 会因产物存在而跳过；否则后处理脚本会打在旧产物上）。
8. 提交信息用中文、按逻辑分组提交；不要提交第三方源码 / 镜像 / 比特流 / 工具链。

## 关键常量（改动前先确认上游有变）

| 项 | 值 |
|---|---|
| 香山 commit | `4b9ddb8a6311bff6f5e29cb3722d1f4236d66292` |
| env-scripts | 基线 `876f39e4cf690745a3951ac6c06ce5475d7d4f16` + `patches/env-scripts/` 12 个补丁 |
| FPGA 器件 | `xcvu19p-fsva3824-1-e`（Vivado 2024.2，license 必需）|
| 板卡时钟 | GC02=80MHz(DDR 参考) · GC04=25MHz(CPU) · GC06=1MHz(RTC)；VADJ=1.2V |
| 内存/UART | DDR 载入 `0x80000000`；UART16550 `0x310B0000`（分频 14 → ≈111.6kbps ≈ 115200）；裸机栈顶 `0x81000000` |
| 板卡资源 | 1Rx8（单 rank）8GB DDR4 SODIMM；FMC2 + UV-FMCH-PDDR4DME 子卡 |

## 仓库地图

```
flows/         ★ 编号流程脚本 00..08（幂等、失败即停、依序执行完成全流程）
patches/       ★ 对上游 env-scripts 的 12 个移植补丁（git am 系列）+ 逐条说明
third_party/   (gitignored) flows/02 拉取：XiangShan@4b9ddb8（浅）+ env-scripts(main+补丁)
hw/            pinmaps/（板卡 IO 与 FMC2-DDR4 引脚表 CSV）· diag/（板卡诊断设计）· README
sw/            baremetal/（裸机源码+Makefile，配方与上板实测产物逐字节一致）· linux/（官方镜像获取+DTB 时钟修正）
tools/         env.sh（环境入口）· setup/00..07（工具链本地化安装，无需 sudo）· checks/（RTL 残留检查）
               · run_hello.sh / run_linux.sh / prog_bitstream.tcl / read_probes.tcl / patch_*.{py,sh,tcl}
docs/          run-guide · onboard-runbook · build-log（完整调试史）· board-setup · board-diag
               · workload-format · rtl-check-report · evidence/（串口实录等上板证据）
build/         (gitignored) 比特流 fpga_top_debug.bit/.ltx、日志、RTL 检查报告
.agents/skills/ 面向 Agent 的技能（见下）
```

## 常用命令

```bash
# 构建链（首次约 30min + RTL 20-40min + 比特流 ~7h）
flows/00_host_check.sh && flows/01_setup_toolchain.sh && flows/02_clone_deps.sh
flows/03_gen_rtl.sh && flows/04_build_bitstream.sh
# 上板（板卡就绪后，约 5 分钟）
flows/05_program_fpga.sh && flows/06_run_hello.sh && flows/07_run_linux.sh
# 排障首选
flows/08_board_diag.sh                    # 板卡外部条件（时钟/复位/串口链路）
vivado -mode tcl -source tools/read_probes.tcl   # 读全部 VIO 探针
```

## 技能（`.agents/skills/`，供 Agent 按需加载）

| 技能 | 用途 |
|---|---|
| `xiangshan-vu19p-bringup` | 从零构建全流程：flows 编排、submodule 浅拉取陷阱与自动修复、RTL 强制重跑与可复现性 |
| `xiangshan-vu19p-board-ops` | 上板运行与排障：VIO/TCL 控制、复位序列、串口/波特率、Linux 镜像 DTB 修正、JTAG 报错（44-152/27-2083/44-49）对照 |
| `baremetal-riscv-hello-first-light` | 裸机 first-light 五坑：栈/build-id/objcopy 段过滤/静态链接/UART 分频 + 负载格式与复现比对 |
| `git-submodule-shallow-clone-traps` | submodule 浅克隆三类陷阱、POSIX foreach、范围最小化、离线镜像验证（通用知识，非本板专属）|

## 验证状态（2026-10-17；详见 README §四）

- ✅ **全链在本仓库内实测通过**（干净环境 → 实板）：依赖/补丁、RTL（与原 RTL
  1578/1579 逐字节一致）、综合（1h51m，0 errors/0 critical）、布线（WNS +0.469ns，
  与原构建一致）、比特流、烧录、裸机 Hello、官方 Linux（OpenSBI → 内核 → buildroot
  shell 可交互，`uname -a`/`ls /` 通过）。实录见 `docs/evidence/*freshbuild*`。
- ✅ 新克隆端到端烟测（GitHub clone → flows/00 → flows/02 → 12 补丁 `git am`）。
- ⚠️ 上板实测中发现并修复的交付缺陷：`setup/02-mill.sh`（全新环境缺 launcher）、
  `prog_bitstream.tcl`/`read_probes.tcl`（Tcl 误用 bash 展开）、`flows/05`（缺成功校验）、
  `run_hello.sh`/`run_linux.sh`（stty 未关 echo → 串口自环回）。
- ▶️ 未重跑：`flows/08` 板卡诊断（原工程实测通过）；`setup/01-02` 的下载路径（URL 未变）。
