# AGENTS.md — 本仓库工作指南

> 人类文档入口：[README.md](README.md)。当前分支 `xvbbp`，远程
> `git@github.com:bupt-openxg/xiangshan-vu19p-fmc2.git`。

## 这是什么

香山昆明湖 `MinimalConfig` 单核 + XVBBP 7 条 Q15 复数向量指令，部署到
PHINE PD-VU19P-2-LSI SOLO v3，运行 BUPT `riscv-xvbbp` 的 FFT/MIMO 裸机负载。
**没有 Linux。**

**综合/实现/比特流由独立构建机完成**；源码、overlay、RTL 生成、软件构建、烧录、
源码、RTL 生成、软件构建、烧录和测试在烧录机完成；综合/实现可交给独立的构建机。

## 黄金规则

1. 板卡操作只走 JTAG + VIO/Tcl，不碰板上按键；串口 115200 8N1，`stty` 必须 `-echo`。
2. 同一时刻只允许一个 Vivado 会话占用 JTAG。
3. `third_party/` 和 `build/` 不入库；不要提交 bit/ltx/log/固件。
4. 官方源码 + overlay（**不是 patch**）：XiangShan 4 + VIMacU 2 行；YunSuan 10。
5. 唯一非官方库是 `bupt-openxg/riscv-xvbbp`；不要引入 GaryCAICHI 依赖。
6. 不要在仓库里复制 BUPT 已有文件；构建时从 `third_party/riscv-xvbbp` 读取。
7. 生成 RTL 前 XiangShan/YunSuan 工作树必须干净（香山把 dirty 写进 RTL）。
8. 测试只保留 BUPT 的 48 FFT + 12 MIMO；不要加自定义 52 例/随机/140 项/性能脚本。

## 关键常量

| 项 | 值 |
|---|---|
| XiangShan | `0a150819d1d1aca1f351975d455d3102cbc2bc24` + BUPT overlay |
| YunSuan | `996b51d86a7df502163a9e89ea3e23ff4b6cc4f9` + BUPT overlay |
| nexus-am | `086a238062fdfb3281b745a537b361983d0154b5` + 板级 overlay |
| env-scripts | `876f39e4cf690745a3951ac6c06ce5475d7d4f16` + 13 板卡补丁 + 16GB DDR overlay |
| riscv-xvbbp | `c55392867c9d7c99c1677f49d6cfa0652a0e2be3` |
| CPU 时钟 | **30 MHz**（内部 MMCM，25 MHz 参考；`DEBUG_CLK_IN` 40 ns） |
| DDR 参考 | 80 MHz（`clk7` / GC02 / `InputClockPeriod 12500`） |
| DDR | 16GB 1Rx8，`MT40A2G8VA-062E`，1 rank，64-bit，AXI 34 位，1600 MT/s |
| UART | 16550 @ `0x310b0000`，divisor 16 |
| 器件 | `xcvu19p-fsva3824-1-e`，Vivado 2024.2 |
| 工具链 | xPack GNU RISC-V GCC 15.2（`tools/setup/08`） |

## 流程

```bash
flows/00_host_check.sh
flows/01_setup_toolchain.sh
flows/02_clone_deps.sh          # 官方 clone + BUPT overlay + 板级 overlay
flows/03_gen_rtl.sh             # make verilog + patch_rtl + rtl_check
flows/04_package_server.sh      # 只打包 RTL/env-scripts/build_server.tcl
# → 构建机：make project + build_server.tcl（synth/impl/bit）
flows/05_program_fpga.sh        # FPGA_BIT_HOME=... 指定 bit/ltx
tests/bupt/ddr16_check.tcl      # 16GB 读写，DDR16G_PASS
flows/09_build_bupt_sw.sh       # 48 FFT + 12 MIMO
flows/10_run_bupt.sh            # 上板并判定
```

## 常见坑

- `DifftestMacros.svh` 不在 include_dirs；`flows/04` 会复制到 `rtl/rtl/`。
- FFT 的 `oai_dfts.o` 必须 `-march=rv64gc`，否则 SIMDe SSE dft16 映射到 RVV 卡死。
- AM 必须 `CPPFLAGS=-DUART16550=1`，ABI lp64d，`mstatus.VS=Initial`。
- 综合/实现只在构建机跑；`report_power` 是 impl 前置步骤。
- `DDR_RANK_WIDTH=1` 是 16GB 1Rx8；2 是 2Rx8，不要混。
- 改 DDR 配置后必须重新综合，不能复用旧 bit。

## 目录

```
flows/      00..10（04 只打包，综合在构建机）
overlays/   board/{nexus-am,env-scripts} · local/xiangshan/VIMacU.scala
tests/      bupt/{build.sh,run.sh,ddr16_check.tcl} · common/
patches/    env-scripts/ 板卡 13 个补丁
tools/      env.sh · setup/ · patch_rtl.sh · checks/ · build_server.tcl
hw/         引脚表 / 板卡诊断
```
