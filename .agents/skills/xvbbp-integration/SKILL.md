---
name: xvbbp-integration
description: 在"上游香山 + 小补丁 + YunSuan 包"方式下集成/维护 XVBBP 7 条自定义向量指令、构建并运行 FFT/MIMO 负载时使用。覆盖补丁来源与再生、YunSuan 包固定、GCC15 工具链、AM 板级适配、上板判据。触发词：xvbbp、Zvbaseband、vscmul、vscmacc、vscaddrq、FFT、MIMO、nexus-am、Vimac、VialuF。
---

# XVBBP 集成与负载（本仓库视角）

## 不可变事实

- 香山基线：`OpenXiangShan/XiangShan@0a150819d1d1aca1f351975d455d3102cbc2bc24`
  （2026-01-23；XVBBP 分支合并的上游父提交）。**不要**用旧的 4b9ddb8a 回移——
  decode API 不同且 YunSuan 数据通路对不上。
- 核心补丁：`patches/xiangshan-xvbbp/0001-xvbbp-core.patch`（5 文件、约 50 行：
  Instructions / VecDecoder / VecExceptionGen / DecodeUnit / VIMacU）。
  来源 fork 提交 `GaryCAICHI/XiangShan@e34d8adb`；再生命令见该目录 README。
- YunSuan：`GaryCAICHI/YunSuan@b4a62448`（分支 `kunminghu-v2-xvbbp`）。
  **V 的运算数据通路在这里**（VIMac64b 复乘/乘累加、VIAluF 复数加/RQ/RTQ），
  与分支先行的 VIMac64b 重构绑定，不要试图拆成对上游 YunSuan 的小补丁。
- 软件：`GaryCAICHI/nexus-am@9dc5f1e1` + `bupt-openxg-lab/riscv-xvbbp@c553928`
  （FFT/MIMO）。AM 入口 `0x80000000`，UART `0x310b0000`。
- 配置 `MinimalConfig`（8-wide / ROB48 / L1D 32KB / L2 128KB×2 / L3 4MB）。

## 五个必踩的坑（都已修，改动前先读）

1. **工作树必须干净**：flows/02 在香山仓库内把补丁 + yunsuan gitlink 落成本地提交
   （如 `local: xvbbp core support ...`）。香山把 dirty 写进生成 RTL。
2. **AM 必须用 xPack GCC 15**（`tools/setup/08`）：
   - GCC 13.3 不认上游 4 参数 `__riscv_vssra_vx_i32m1(..., vxrm, vl)` → MIMO 编译失败；
   - xPack 默认 ABI 是 ilp32 → `patches/nexus-am/0003` 显式 `-mabi=lp64d`；
   - AM 源码按 50MHz 算 UART 分频 → `patches/nexus-am/0001` 改 14（25MHz）；
   - AM 只置了 `mstatus.FS` → `patches/nexus-am/0002` 置 `VS=Initial`。
3. **构建 AM 镜像必须 `CPPFLAGS=-DUART16550=1`**（flows/09 已内置）：本板 UART 是
   16550 @ `0x310b0000`；默认 uartlite @ `0x40600000` 不存在，CPU 首次 putchar
   即挂死（串口全空、VIO 的 cfg/ddr/uart 计数器冻结）。该宏不在 AM Makefile 依赖里，
   切换后端要清 `am/build`（flows/09 每次清）。
4. **FFT 的 OAI 校验参考必须标量化**（`patches/nexus-am/0004`）：ext/rvv 镜像用
   `-march=rv64gcv` 时 SIMDe 把参考的 SSE `dft16` 映射成 RVV，本核上重复调用约
   5 次后卡死；只把 `oai_dfts.o` 编成 `-march=rv64gc`。EXT/RVV 内核本身正常，
   `smoke`（n16）和 scalar 基线可作为对照。
5. **MIMO 产物按后端同名**：`mimo-ext.bin`/`mimo-scalar.bin`，不同 kernel 会互相覆盖；
   flows/09 每次构建后立刻复制到 `build/xvbbp/<key>.bin` 并写 `images.tsv`。

## 流程

```bash
flows/00_host_check.sh
flows/01_setup_toolchain.sh       # 含 02 mill(0.12.15 launcher)、07 libc 头、08 xPack GCC15
flows/02_clone_deps.sh            # 香山+yunsuan 固定+核心补丁+nexus-am+riscv-xvbbp+env-scripts
flows/03_gen_rtl.sh               # make verilog CONFIG=MinimalConfig JVM_XMX=20G
flows/04_build_bitstream.sh       # 约 4-7h（综合峰值 ~32GB；实现建议 ≥64GB）
flows/05_program_fpga.sh
flows/06_run_hello.sh             # 回归
flows/09_build_xvbbp_sw.sh        # 默认代表集；ALL=1 构建官方 48 FFT + 12 MIMO
flows/10_run_xvbbp.sh smoke       # 期望 EXT instruction demo: PASS
flows/10_run_xvbbp.sh fft-ext-1024
flows/10_run_xvbbp.sh fft-ext-4096
flows/10_run_xvbbp.sh mimo-ext-zf4
flows/10_run_xvbbp.sh mimo-ext-mmse2
flows/11_build_am_bench.sh        # Dhrystone / CoreMark / MicroBench（本板 16550）
flows/10_run_xvbbp.sh build/am-bench/coremark-2iter.bin
MATRIX=1 flows/09_build_xvbbp_sw.sh && flows/12_run_xvbbp_matrix.sh
# ↑ EXT 复杂矩阵：8 尺寸 × 正/逆 FFT + 4 MIMO kernel（约 40 分钟）
```

## 验证要点（2026-10-20 全链已通过）

- RTL 中应能 grep 到 `Zvbaseband`/`VSCADDRTQ`（decode）与 VIMacU 的 `isComp/isConj`。
- `flows/10` 判据：smoke/fft-ext 需 `EXT instruction demo: PASS` + `CHECK: PASS`；
  fft-scalar 需 `CHECK: PASS`；mimo 需 `MIMO_VALIDATE_PASS` + `MIMO_DONE`。
- 实测：smoke 7 条指令逐条 PASS；fft-ext n1024/n4096 CHECK PASS；
  mimo-ext zf4 fail=0 max_abs=10/11、mmse2 fail=0 max_abs=7/11；scalar 基线通过。
  证据：`docs/evidence/xvbbp-20261020/`。
- NEMU/AM 复杂测试（flows/11）：Dhrystone 500k runs `PASS`（2.23 DMIPS/MHz、IPC 1.643）；
  CoreMark 2K CRC 全匹配（`crcfinal 0x72be`）；MicroBench test 规模 10 项全 `Passed.`。
  证据：`docs/evidence/am-bench-20261020/`。`ready-to-run/*.bin` 是 uartlite 版，
  本板不能直接用。
- XVBBP EXT 复杂矩阵（MATRIX=1 + flows/12）：**21/21 PASS**——8 尺寸 × 正/逆 FFT
  （逆变换真实执行共轭指令）+ ZF/MMSE 2x2/4x4（fail=0，max_abs≤10/11）。
  证据：`docs/evidence/xvbbp-matrix-20261020/`。只跑默认代表集会漏掉逆变换的共轭通路。
- MIMO 的 `.bin` 因 `.bss` 置零约 1–4MB，JTAG 写入是分钟级，不是卡死。
- `dft_rvv` 后端依赖香山对相应 RVV 指令的实现，不作为硬性目标；ext/scalar 是必跑项。

## 排障

- `flows/02` 报“不是从 0a150819 派生的”：`rm -rf third_party/XiangShan` 重跑（旧 V2/V3 克隆不兼容）。
- `setup/02` 显示 `mill --version` 为 1.3.0：官方 launcher 在无 `.mill-version` 的临时目录的默认值；
  在 XiangShan 目录会用 0.12.15（首次下载到 `~/.cache/mill`），属正常。
- `unable to find library -lriscv` / 头文件缺失：`tools/setup/07` 未装
  `libgcc-13-dev/libc6-dev/linux-libc-dev-riscv64-cross`。
- 串口全空且 VIO 的 `dbg_cfg_ar_cnt/dbg_ddr_ar_cnt/dbg_uart_tx_edges` 停止增长：
  镜像没带 `-DUART16550=1`，CPU 卡在 uartlite 地址上；重建（flows/09 已内置）。
- FFT 打印完 demo 后停在 `dft16`/参考计算：确认 `oai_dfts.o` 以 `-march=rv64gc`
  编译（`patches/nexus-am/0004`），可用 `objdump -d | grep -c '^ *v'` 快速核对。
