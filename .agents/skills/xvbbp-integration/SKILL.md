---
name: xvbbp-integration
description: 在官方 XiangShan/YunSuan 上以 BUPT overlay 集成 XVBBP 7 条 Q15 复数向量指令、构建并运行 BUPT FFT/MIMO 负载时使用。覆盖 overlay 文件清单、GCC15/AM 板级适配、16GB DDR、构建机综合与上板判据。触发词：xvbbp、vscmul、vscmacc、vscaddrq、FFT、MIMO、nexus-am、Vimac、VialuF。
---

# XVBBP 集成与负载（本仓库视角）

## 版本元组（唯一正确组合）

- XiangShan：`OpenXiangShan/XiangShan@0a150819`（官方）
- YunSuan：`OpenXiangShan/YunSuan@996b51d`（官方）
- BUPT：`bupt-openxg/riscv-xvbbp@c553928`（唯一非官方库）
  - `hardware/xiangshan/overlay` 4 个 decode 文件
  - `hardware/yunsuan/overlay` 10 个执行文件
- 本地 glue：`overlays/local/xiangshan/VIMacU.scala`
  （官方文件 + `mod.io.isComp/isConj := VimacOpcode.isComp/isConj(opcode)`）
- nexus-am：`OpenXiangShan/nexus-am@086a238` + `overlays/board/nexus-am`
- env-scripts：`OpenXiangShan/env-scripts@876f39e` + 13 个板卡补丁 +
  `overlays/board/env-scripts`（16GB DDR）
- 工具链：xPack GNU RISC-V GCC 15.2（`tools/setup/08`）

**不要**再使用 GaryCAICHI fork、`patches/xiangshan-xvbbp` 或 patch 模式。
`flows/02` 会校验 XiangShan 相对官方只差 5 个源文件 + yunsuan gitlink。

## 7 条指令

opcode `0x57`，funct3 `000`，SEW=16，固定寄存器 vd=v16 / vs2=v20 / vs1=v24：

| funct6 | 指令 | 语义 |
|---|---|---|
| 0x32 | vscmul | `(ar·br−ai·bi, ar·bi+ai·br)` |
| 0x33 | vscmulcj | `(ar·br+ai·bi, ar·bi−ai·br)` |
| 0x34 | vscmacc | vscmul + `vd<<15` 后按 vxrm 舍入饱和 |
| 0x36 | vscmacccj | vscmulcj + `vd<<15` |
| 0x37 | vscaddconj | `(ar+br, bi−ai)` |
| 0x38 | vscaddrq | `(br−ai, bi+ar)` |
| 0x39 | vscaddrtq | `(br+ai, bi−ar)` |

编码：`0xCB4C0857/0xCF4C0857/0xD34C0857/0xDB4C0857/0xDF4C0857/0xE34C0857/0xE74C0857`。

## 必须知道的坑

1. 生成 RTL 前 XiangShan/YunSuan 工作树必须干净（香山把 dirty 写进 RTL）。
2. `DifftestMacros.svh` 不在 `cpu_files.tcl` 的 include_dirs；`flows/04` 复制到
   `build/rtl/DifftestMacros.svh`，打包后在 `rtl/rtl/`。
3. AM 镜像必须 `CPPFLAGS=-DUART16550=1`（16550 @ `0x310b0000`，divisor 14）。
4. FFT 的 `oai_dfts.o` 必须 `-march=rv64gc`；SIMDe SSE dft16 若编成 RVV，
   本核重复调用会卡死。EXT/RVV 内核本身仍用 `-march=rv64gcv`。
5. MIMO 必须 xPack GCC 15；ABI `lp64d`；`mstatus.VS=Initial`。
6. 16GB 1Rx8 用 `MT40A2G8VA-062E`（Components，AXI 34），不是 2Rx8 SODIMM
   preset，也不是 `DDR_RANK_WIDTH=2`。CPU 侧地址会先减 `0x80000000` 再进 MIG。
7. 综合/实现只在大内存构建机执行；`report_power` 是 impl 前置步骤。

## 流程

```bash
flows/02_clone_deps.sh
flows/03_gen_rtl.sh
flows/04_package_server.sh      # 上传 build/bupt-server-*/
# 构建机：make project + build_server.tcl
FPGA_BIT_HOME="$BIT_DIR" flows/05_program_fpga.sh
vivado -mode batch -source tests/bupt/ddr16_check.tcl -tclargs .../fpga_top_debug.ltx
flows/09_build_bupt_sw.sh
FPGA_BIT_HOME=... flows/10_run_bupt.sh
```

## 判定

- DDR：`DDR16G_PASS`（2/6/10/14GB 和 16GB 末尾）。
- FFT：`CHECK: PASS`；ext 镜像还要 `EXT instruction demo: PASS`。
- MIMO：`MIMO_VALIDATE_PASS` + `MIMO_DONE status=0`。
- 汇总：`BUPT_RESULT total=60 pass=60 fail=0`。
