# tests/ —— 课题二测试方案（任务一）的仓库内移植

本目录把《02. 重点研发计划-课题二测试方案-任务一-20260818》中的测试用例
移植到 `ReConstruct`，并给出在 PD-VU19P-2-LSI SOLO v3 开发板上直接执行的脚本。
官方测试源码取自测试分支（见 `upstream.env`），板级运行复用本仓库已验证的
JTAG/VIO + UART16550 流程。

## 目录

| 路径 | 对应文档章节 | 内容 |
|---|---|---|
| `ext-instruction-test/` | 测试用例一 / 流程二 | KMHv2 整核给定激励，52 用例 |
| `random/` | 测试用例一 / 流程一 | 7 条扩展指令的核级随机激励（板级可执行） |
| `fft/` | 测试用例二 / 流程一、二 | FFT 功能验证（140 项）与性能（14 组） |
| `mimo/` | 测试用例二 / 流程一、二 | MIMO 功能验证（16 项）与性能（4 组） |
| `common/` | — | 构建环境、单次上板启动、串口抓取 |

`*/app/` 是从官方 `nexus-am` 测试分支原样复制的裸机程序（保留 OAI/上游
许可证）。唯一的源码改动是 `fft/app/Makefile` 中把 `oai_dfts.o` 单独编成
`-march=rv64gc`（见 `patches/nexus-am/0004-fft-oai-scalar-reference.patch`），
因为 SIMDe 的 SSE dft16 在 `-march=rv64gcv` 下会映射到 RVV，而该参考实现
在本核上重复调用会卡死；标量参考的逐位结果不变。

## 运行前提

- 已烧录 `build/fpga_top_debug.bit`（`flows/05_program_fpga.sh`）。
- 串口 `/dev/ttyUSB0`，115200 8N1；可用 `SER=` 覆盖。
- xPack GCC 15（`tools/env.sh` 自动设置）。
- 首次运行会构建 `third_party/nexus-am` 的 `am`/`klib`；必须使用
  `CPPFLAGS=-DUART16550=1`（本板 16550 在 `0x310b0000`，默认 uartlite 会挂死）。
  脚本已内置该宏；手工构建时若切换过串口后端，先 `rm -rf third_party/nexus-am/am/build`。

## 命令

```bash
# 52 个整核给定激励用例
tests/ext-instruction-test/run.sh

# 7 条扩展指令随机激励（默认 100 万条；可调规模）
RANDOM_CASES=10000000 tests/random/run.sh

# FFT 功能验证（7 点 × 2 方向 × 5 输入 × 2 实现 = 140）
tests/fft/run_functional.sh
# FFT 性能（14 组，RVV vs RVV+EXT，阈值 30%）
tests/fft/run_performance.sh

# MIMO 功能验证（2x2 三种调制 + 4x4，共 16 项）
tests/mimo/run_functional.sh
# MIMO 性能（2x2/4x4 × ZF/MMSE，共 4 组，阈值 30%）
tests/mimo/run_performance.sh
```

每个脚本在 `build/tests/<suite>/` 下写 `summary.log`、`cases.tsv` 和逐用例
`build.log`/`run.log`；串口原始输出已去掉 CR。`cases/` 的目录层次与官方
验收脚本一致（点数/方向/输入/实现）。

单次上板由 `common/board_run.sh` 完成：一次 Vivado 会话内 `halt → JTAG 写
DDR → VIO 复位`，串口同时抓取，检测到 `Exit with code =` 后提前结束，
平均约 10–15 秒/用例。

## 与文档的差异（必须在报告中说明）

1. **运行平台**：文档的整核测试用 XiangShan `emu`，FFT/MIMO 功能用 NEMU；
   本移植把同一批裸机镜像放到 FPGA 上运行。整核 52 用例的判定改为
   `SUMMARY: 52/52 cases passed` + `EXT instruction fixed-vector test: PASS`
   + `Exit with code = 0`（板级 AM 的正常结束标志；NEMU 下对应
   `HIT GOOD TRAP`）。
2. **功能单元随机激励**：文档要求 VIMAC/VIALUF 各 10⁹ 条、覆盖 20/75 条
   指令，依赖 `XiangShan/yunsuan/build/emu`（Verilator）。板卡无法运行该
   emu；本目录提供：
   - `random/`：7 条扩展指令的核级随机差分，e16/m1/vl=8，vxrm 0–3 全遍历，
     默认 10⁶ 条，可放大到 10⁷–10⁸（板级实测约 24 秒/10⁶ 条）；
   - `random/run_random_tests.py`：官方 FU 级脚本的移植副本，供有
     Verilator 的环境按文档原样运行 10⁹ 条。
   报告中把前者作为“FPGA 可执行验证”，后者作为“仿真环境复现入口”，
   不把 10⁶ 条冒充 10⁹ 条。
3. **性能统计**：文档用整核 emu 的 HPM；板级用程序内 `rdcycle` 的 ROI
   区间（FFT/MIMO 程序自带，统计边界与文档一致：只含变换/均衡计算）。
   测试程序仍打印 `FFT_ROI`/`MIMO_ROI`，按 `repeat` 取单次平均。

## 上游来源

见 `upstream.env`。`app/` 内文件保持上游内容；如需重新同步，按该文件中的
仓库和 commit 重新复制，并重新应用 `patches/nexus-am/0004`。
