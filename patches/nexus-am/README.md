# nexus-am 移植补丁（GaryCAICHI/nexus-am）

基线：`GaryCAICHI/nexus-am@9dc5f1e19b3e0dc58beade6f9a2e25dc90a6f2b8`（XVBBP 冻结软件包
`bupt-openxg-lab/riscv-xvbbp` 指定的 AM fork 版本）。`flows/02_clone_deps.sh` 拉取后自动应用。

| # | 主题 | 原因 |
|---|---|---|
| 0001 | **UART 分频改 25MHz** | 上游 `serial-16550.c` 按 `divisor = 50000000/(16*115200)`（=27）初始化；本板 UART 参考时钟是 **25MHz**（GC04），照 27 分频只有 ~57.9kbps → 串口乱码。改为 `divisor = 14`（25e6/(16*115200)=13.56，取 14 ≈111.6kbps），与本仓库裸机 Hello 的时序一致。UART 基址 `0x310b0000` 与板卡一致，无需改 |
| 0002 | **使能 mstatus.VS** | AM 的 `start.S` 只置了 `mstatus.FS`（浮点）；FFT/MIMO 的 EXT/RVV 路径使用向量指令，规范要求 `mstatus.VS != Off`。补丁在 FP 初始化后置 `VS=Initial`（`csrs mstatus, 0x200`）|
| 0003 | **显式 `-mabi=lp64d`** | xPack `riscv-none-elf-gcc` 的默认 ABI 是 ilp32；`mainargs.S` 等汇编只带 `-march` 时 cc1 报 `ABI requires '-march=rv32'`。在 `am/arch/isa/riscv64.mk` 的 `COMMON_FLAGS` 里显式加 `-mabi=lp64d`（对旧的 linux-gnu 工具链同样正确）|
| 0004 | **FFT 校验参考标量化** | `apps/fft/Makefile`：把 `oai_dfts.o` 单独编成 `-march=rv64gc`。FFT 的 EXT/RVV 镜像使用 `-march=rv64gcv` 时，SIMDe 会把 OAI 参考的 SSE `dft16` 映射为 RVV；该参考在本核上重复调用约 5 次后卡死（`vlse64/vslideup/vrgather` 序列），而 ext/rvv 内核本身正常。标量参考与向量内核逐位一致，仅影响校验路径 |

应用方式（`flows/02` 自动、幂等；0004 由 `flows/09` 在每次从冻结包覆盖 apps 后应用）：

```bash
git -C third_party/nexus-am apply patches/nexus-am/0001-uart-25mhz.patch
git -C third_party/nexus-am apply patches/nexus-am/0002-enable-vector-state.patch
git -C third_party/nexus-am apply patches/nexus-am/0003-explicit-lp64d-abi.patch
# 0004 作用于 apps/（flows/09 拷贝官方 apps 之后）：
patch -p1 -d third_party/nexus-am/apps --batch \
      patches/nexus-am/0004-fft-oai-scalar-reference.patch
```

构建镜像还必须带 **`CPPFLAGS=-DUART16550=1`**（`flows/09` 已内置）：本板 UART 是
16550 @ `0x310b0000`，AM 默认的 uartlite @ `0x40600000` 不存在，CPU 首次 putchar
即挂死。该宏未纳入 AM 的依赖跟踪，切换后端时需清 `am/build`（`flows/09` 已做）。

验证：`flows/09_build_xvbbp_sw.sh` 构建出的镜像在板上应打印
`EXT instruction demo: PASS`、`CHECK: PASS` 与 `MIMO_VALIDATE_PASS` / `MIMO_DONE`。
2026-10-20 的实测记录见 `docs/evidence/xvbbp-20261020/`。
