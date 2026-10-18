# nexus-am 移植补丁（GaryCAICHI/nexus-am）

基线：`GaryCAICHI/nexus-am@9dc5f1e19b3e0dc58beade6f9a2e25dc90a6f2b8`（XVBBP 冻结软件包
`bupt-openxg-lab/riscv-xvbbp` 指定的 AM fork 版本）。`flows/02_clone_deps.sh` 拉取后自动应用。

| # | 主题 | 原因 |
|---|---|---|
| 0001 | **UART 分频改 25MHz** | 上游 `serial-16550.c` 按 `divisor = 50000000/(16*115200)`（=27）初始化；本板 UART 参考时钟是 **25MHz**（GC04），照 27 分频只有 ~57.9kbps → 串口乱码。改为 `divisor = 14`（25e6/(16*115200)=13.56，取 14 ≈111.6kbps），与本仓库裸机 Hello 的时序一致。UART 基址 `0x310b0000` 与板卡一致，无需改 |
| 0002 | **使能 mstatus.VS** | AM 的 `start.S` 只置了 `mstatus.FS`（浮点）；FFT/MIMO 的 EXT/RVV 路径使用向量指令，规范要求 `mstatus.VS != Off`。补丁在 FP 初始化后置 `VS=Initial`（`csrs mstatus, 0x200`）|
| 0003 | **显式 `-mabi=lp64d`** | xPack `riscv-none-elf-gcc` 的默认 ABI 是 ilp32；`mainargs.S` 等汇编只带 `-march` 时 cc1 报 `ABI requires '-march=rv32'`。在 `am/arch/isa/riscv64.mk` 的 `COMMON_FLAGS` 里显式加 `-mabi=lp64d`（对旧的 linux-gnu 工具链同样正确）|

应用方式（flows/02 自动、幂等）：

```bash
git -C third_party/nexus-am apply patches/nexus-am/0001-uart-25mhz.patch
git -C third_party/nexus-am apply patches/nexus-am/0002-enable-vector-state.patch
```

验证：`flows/09_build_xvbbp_sw.sh` 构建出的镜像在板上应打印
`EXT instruction demo: PASS` 与 `MIMO_VALIDATE_PASS` / `FFT_*` 输出。
