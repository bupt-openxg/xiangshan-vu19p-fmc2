# XVBBP：香山昆明湖 V 扩展集成与 FFT/MIMO 负载

> 本文记录把 `bupt-openxg-lab/riscv-xvbbp`（Cai Chi 等设计的 7 条基带向量指令）
> 以“**上游香山 + 小补丁 + YunSuan 包**”的方式集成到本仓库的过程、构建与上板方法。
> 适用板卡与主流程同 [README](../README.md)：PHINE PD-VU19P-2-LSI SOLO v3，Vivado 2024.2。

## 1. 结论（解耦评估）

| 层次 | 改动 | 是否可分离 |
|---|---|---|
| 香山译码/异常/发射 | **5 个 Scala 文件、约 50 行**（见 `patches/xiangshan-xvbbp/`）| ✅ 可分离：与上游 `DecodeInUop/DecodeOutUop` API 无耦合 |
| 运算数据通路 | YunSuan 包（VIMac64b 复乘/乘累加、VIAluF 复数加、共轭、饱和 RQ/RTQ）| ⚠️ 与 YunSuan 分支先行的 VIMac64b 重构绑定，**宜作为 Scala 包固定 SHA**，不拆成小补丁 |
| 软件 | FFT/MIMO（nexus-am，`riscv64-xs` 目标）| ✅ 独立；仅需板级 UART/时钟适配 |

因此集成方案是：

- **香山内核**：`OpenXiangShan/XiangShan@0a150819d1d1aca1f351975d455d3102cbc2bc24`
  （2026-01-23，正是 XVBBP 分支合并的上游父提交；
  `flows/02` 浅拉取 + `patches/xiangshan-xvbbp/0001-xvbbp-core.patch` + 本地提交）
- **YunSuan 包**：`GaryCAICHI/YunSuan@b4a62448b1539ab875daf36a62fe9cf6bc2fbe7b`
  （分支 `kunminghu-v2-xvbbp`；`flows/02` 显式固定并记录到香山本地提交）
- **软件**：`GaryCAICHI/nexus-am@9dc5f1e1` + `bupt-openxg-lab/riscv-xvbbp@c553928` 的 apps
- **板级**：本仓库 `patches/env-scripts/`（12 个补丁，原 V2 流程已验证）+ `patches/nexus-am/`（3 个）
- 配置：`MinimalConfig`（8-wide / ROB48 / L1D 32KB / L2 128KB×2 / L3 4MB）

> 官方板级仓库 `bupt-openxg-lab/xiangshan-vu19p-fmc2` 计划稍后发布；本方案不依赖它，
> 发布后可对照其 FPGA 配置与实测数据。

## 2. 七条指令

编码：opcode `1010111`（OPIVV）、`funct3=000`、`SEW=16`（Q15 定点，饱和写 `vxsat`）。

| 指令 | funct6 | 译码去向 | 语义（简要） |
|---|---|---|---|
| `vscmul.vv` | 110010 | `vimac` / `VimacType.vscmul` | 复数乘（Q15 饱和）|
| `vscmulcj.vv` | 110011 | `vimac` / `vscmulcj` | 乘 + 对 vs2 共轭 |
| `vscmacc.vv` | 110100 | `vimac` / `vscmacc` | 复数乘累加 |
| `vscmacccj.vv` | 110110 | `vimac` / `vscmacccj` | 乘累加 + 共轭 |
| `vscaddconj.vv` | 110111 | `vialuF` / `vscaddconj` | 复数加 + 共轭 |
| `vscaddrq.vv` | 111000 | `vialuF` / `vscaddrq` | 饱和 RQ 旋转 |
| `vscaddrtq.vv` | 111001 | `vialuF` / `vscaddrtq` | 饱和 RTQ 旋转 |

数据按 `e16,m1`、`vl` 个 16-bit lane 处理。官方最小自检：`fft dft_ext n16`
（`kernels/ext/n16/dft_ext.c` 的 `run_ext_instruction_demo()`），成功打印
`EXT instruction demo: PASS`。

## 3. 仓库集成（flows/02 自动完成）

```bash
git switch -c xvbbp          # 从 kmh-v2 拉出（kmh-v2/kmh-v3 不动）
flows/00_host_check.sh
flows/01_setup_toolchain.sh  # 追加安装 xPack GCC 15（tools/setup/08，约 413MB 下载）
flows/02_clone_deps.sh
```

`flows/02` 的关键动作：

1. 浅拉香山 `0a150819`；初始化 11 个顶层 submodule（含 `yunsuan`、`ChiselIOPMP`）
   与 3 个嵌套（`rocket-chip/{cde,hardfloat}`、`openLLC/openNCB`），并自动修复
   浅克隆的两类 submodule 陷阱。
2. 把 `yunsuan` 本地 URL 指向 XVBBP fork，`fetch --depth 1` 并 checkout `b4a62448`。
3. `git apply --index patches/xiangshan-xvbbp/0001-xvbbp-core.patch`。
4. 在香山仓库内 `git add -A` + 本地提交（**必须**：香山会把 dirty 写进 RTL；
   该提交同时记录 yunsuan 的 fork SHA）。提交后工作树必须干净。
5. 拉取 `nexus-am@9dc5f1e1` 并应用 `patches/nexus-am/`：
   - `0001` UART 分频 25MHz（上游按 50MHz 写成 27，板上会变成 ~57.9kbps 乱码）
   - `0002` `mstatus.VS=Initial`（向量指令前置条件）
   - `0003` `-mabi=lp64d`（xPack GCC 15 默认 ABI 是 ilp32）
6. 拉取 `riscv-xvbbp@c553928`（FFT/MIMO apps 源）。

补丁的逐行来源与再生命令见 `patches/xiangshan-xvbbp/README.md`。

## 4. RTL 与综合

```bash
flows/03_gen_rtl.sh          # make verilog CONFIG=MinimalConfig JVM_XMX=20G
flows/04_build_bitstream.sh  # 工程 + 综合 + 实现 + 比特流（约 7h）
```

- 0a150819 的 Makefile 在非 `FPGA=1` 时自动附加
  `--fpga-platform --reset-gen --ignore-read-enable-mem`；顶层端口仍有
  `io_riscv_halt_0/io_dma_*`，沿用 `patches/env-scripts/` 的 V2 路径。
- 8-wide 后期线路的综合 elaboration 工作集约 90–110GB。**15GB 机器只会 swap 抖动**；
  推荐 ≥64GB RAM（128GB 更稳）。本仓库会在本机尝试并在 `docs/evidence/` 记录结论；
  若受阻，`third_party/` 可整体复制到大机器后用同一组 flows 重建。

## 5. 裸机负载构建与运行

```bash
flows/09_build_xvbbp_sw.sh           # 默认代表集（见下）
ALL=1 flows/09_build_xvbbp_sw.sh     # 另外构建官方全量 48 FFT + 12 MIMO（不逐镜像归档）
flows/05_program_fpga.sh             # 烧录比特流
flows/06_run_hello.sh                # 回归：裸机 Hello
flows/10_run_xvbbp.sh smoke          # 7 指令冒烟，期望 EXT instruction demo: PASS
flows/10_run_xvbbp.sh fft-ext-1024
flows/10_run_xvbbp.sh mimo-ext-zf4   # 期望 MIMO_VALIDATE_PASS / MIMO_DONE
```

`flows/09` 产出的 key（`build/xvbbp/images.tsv`）：

| key | 内容 |
|---|---|
| `smoke` | FFT EXT n16（官方七指令自检）|
| `fft-ext-1024` / `fft-ext-4096` | EXT 后端 FFT |
| `fft-scalar-1024` | 标量后端参考 |
| `mimo-ext-zf4` / `mimo-ext-mmse2` | EXT 后端 MIMO（ZF / MMSE）|
| `mimo-scalar-zf4` | 标量参考 |

运行机制（`flows/10`）：`bin2fpgadata` 转格式 → `make halt_soc` → `make write_ddr`
（DDR 0x80000000）→ 串口抓包 → `make reset_cpu`。AM 镜像入口固定为 `0x80000000`
（`loader64.ld`），栈在 1GB RAM 区顶部，板载 8GB DDR 足够。

判据：`smoke`/`fft-ext-*` 要求 `EXT instruction demo: PASS` 且 `CHECK: PASS`；
`fft-scalar-*` 要求 `CHECK: PASS`；`mimo*` 要求 `MIMO_VALIDATE_PASS` 与 `MIMO_DONE`；
出现 `Mismatch`/`CHECK: FAIL`/`MIMO_VALIDATE_FAIL` 即判失败。

### 软件侧注意事项

- **必须用 xPack GCC 15**（`tools/setup/08`）：上游冻结代码用 GCC 14+ 的 RVV
  intrinsic 签名（如 `vssra` 显式 `vxrm`），本仓库的 GCC 13.3 编译 MIMO 会报
  “too many arguments”。
- AM 镜像的 `.bin` 含 `.bss`（objcopy 置为 alloc,contents），MIMO 约 4MB，
  JTAG 写入比 Hello 慢（分钟级），属正常。
- EXT 代码用 `.word` 原样编码（不依赖编译器识别自定义指令）；
  `dft_rvv` 后端需要香山实现对应 RVV 指令，**不作为本轮硬性目标**。

## 6. 已验证 / 待验证

| 项 | 状态 |
|---|---|
| 核心补丁生成（5 文件、122 行 diff，无无关改动）| ✅ |
| 依赖集成（flows/02：香山+yunsuan+nexus-am+riscv-xvbbp+env-scripts）| ✅ |
| AM 工具链（xPack GCC 15 + 3 个补丁）| ✅ 离线构建通过（FFT n16/n1024/n4096、MIMO ext/scalar）|
| RTL 生成与残留检查（flows/03）| ✅ 1821 文件；残留检查通过；`io_isComp/io_isConj` 已在 VIMac64bStage1/VIMacU |
| 本机综合（flows/04）| ✅ **综合成功**（2h06m，0 errors / 0 critical warnings，峰值 31.9GB；15GB RAM + 128GB swap 可完成）；⏳ 实现/比特流进行中 |
| 板上 Hello 回归 | ⏳ |
| 板上 7 指令冒烟 / FFT / MIMO | ⏳ |

## 7. 许可与致谢

- XVBBP 设计与实现：Cai Chi 等（`GaryCAICHI/xs-env`、`GaryCAICHI/XiangShan`、
  `GaryCAICHI/YunSuan`、`GaryCAICHI/nexus-am`）；迁移与文档：`bupt-openxg-lab/riscv-xvbbp`
  （Apache-2.0 包装脚本；XiangShan/YunSuan 源文件为 MulanPSL-2.0；nexus-am 保留原许可；
  FFT 内置 SIMDe 保留其许可证）。
- 本仓库只保存最小补丁与脚本，第三方源码由 `flows/02` 拉取（`third_party/` 不入库）。
