# sw/xvbbp — XVBBP 裸机负载

本目录不存放源码：7 条指令的官方自检与 FFT/MIMO 工作负载都在
`bupt-openxg-lab/riscv-xvbbp` 的冻结软件包里（Apache-2.0 包装、源文件保留原许可）。
`flows/02_clone_deps.sh` 把包与 `GaryCAICHI/nexus-am` 拉到 `third_party/`，
`flows/09_build_xvbbp_sw.sh` 构建并归档到 `build/xvbbp/`。

## 为什么用官方自检作为“冒烟”

官方最小自检是 `apps/fft/kernels/ext/n16/dft_ext.c` 的 `run_ext_instruction_demo()`：
它用 `vsetivli` + 7 条 `.word` 原样编码，与 C 标量 Q15 参考逐 lane 比对，
并检查 `vxsat`，逐条打印 `EXT demo <name>: PASS`，最后打印
`EXT instruction demo: PASS`。这比自写一份 smoke 更权威。

## 构建与运行

```bash
source tools/env.sh
flows/09_build_xvbbp_sw.sh          # 默认代表集 -> build/xvbbp/images.tsv
flows/10_run_xvbbp.sh smoke         # 七指令冒烟（KEY 可换成 fft-*/mimo-*）
```

直接使用官方脚本构建单个镜像（等价于 flows/09 内部命令）：

```bash
TC="$RISCV_XPACK_PREFIX"
AM=third_party/nexus-am
make -C "$AM/apps/fft" DFT_IMPL=dft_ext FFT_POINTS=16 FFT_RUN_IDFT=0 \
     FFT_VALIDATE=1 FFT_REPEAT=1 ARCH=riscv64-xs AM_HOME="$AM" \
     CROSS_COMPILE="$TC" RVV_TOOLCHAIN_PREFIX="$TC" \
     CPPFLAGS=-DUART16550=1 \
     CC_OPT="-O2 -Wno-error=unused-variable -Wno-error=unused-function"
# -> $AM/apps/fft/build/hello-ext-validate-n16-r1-idft0-riscv64-xs.bin
```

## 工具链

- **必须** xPack `riscv-none-elf-gcc 15.2.0`（`tools/setup/08` 安装）。上游代码使用
  GCC 14+ 的 RVV intrinsic 签名；本仓库的 GCC 13.3 会使 MIMO 编译失败。
- **必须** `CPPFLAGS=-DUART16550=1`：本板 UART 是 16550 @ `0x310b0000`；AM 默认的
  uartlite @ `0x40600000` 不存在，首次 putchar 会挂死 CPU。
- 板级补丁（`patches/nexus-am/`）：UART 分频 25MHz、`mstatus.VS=Initial`、显式
  `-mabi=lp64d`、FFT 的 OAI 校验参考标量化（0004，避免 SIMDe 的 RVV dft16 在
  重复调用后卡死；EXT/RVV 内核仍为 `-march=rv64gcv`）。0004 由 `flows/09` 在拷贝
  官方 apps 后应用。

## 镜像与加载

- AM 入口 `0x80000000`（`loader64.ld`），平铺 `.bin` 由 `bin2fpgadata` 转成 DDR 写入格式。
- `mimo-*.bin` 因 `.bss` 置零约 1–4MB，JTAG 写入为分钟级；`fft-*.bin` 约 125–165KB。
- 期望输出与判据见 `docs/xvbbp.md` §5。
