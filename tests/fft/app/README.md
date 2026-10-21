# FFT 冻结实现

这是 FFT 的最终发布目录。这里只保留 bit-exact FFT/IDFT 的维护主线源码和构建输入，不包含实验候选、archive、evidence、仿真日志或性能报告。

## 配置范围

支持点数：`16`、`64`、`128`、`256`、`512`、`1024`、`2048`、`4096`。

支持实现：

- `dft_scalar`：`kernels/scalar/n<points>/dft_scalar.c`
- `dft_rvv`：`kernels/rvv/n<points>/dft_rvv.c`
- `dft_ext`：`kernels/ext/n<points>/dft_ext.c` 及同目录 RVV companion

每个点数、方向和 backend 只有一个实现。`kernels/mainline.mk` 是唯一映射表；Makefile 强制使用 `FFT_MAINLINE=1`，不接受实验源文件覆盖。

## 构建

先设置 Nexus-AM 根目录：

```bash
export AM_HOME=/path/to/nexus-am
```

以 256 点正变换的 RVV bit-exact 验证为例：

```bash
make -C apps/fft \
  DFT_IMPL=dft_rvv \
  FFT_POINTS=256 \
  FFT_RUN_IDFT=0 \
  FFT_VALIDATE=1 \
  FFT_REPEAT=1
```

EXT 性能构建示例：

```bash
make -C apps/fft \
  DFT_IMPL=dft_ext \
  FFT_POINTS=4096 \
  FFT_RUN_IDFT=0 \
  FFT_VALIDATE=0 \
  FFT_REPEAT=1 \
  FFT_WARMUP_REPEAT=1
```

将 `DFT_IMPL` 换成 `dft_scalar`、`dft_rvv` 或 `dft_ext`，并选择支持的 `FFT_POINTS`。`FFT_RUN_IDFT=1` 运行逆变换；正变换和逆变换必须分别验证。

一次编译全部 48 个冻结构建组合（3 后端 × 8 点数 × 正/逆变换）：

```bash
make -C apps/fft xvbbp-build-all AM_HOME="$AM_HOME"
```

## NEMU 正确性标准

验证构建完成后：

```bash
${AM_HOME}/NEMU/build/riscv64-nemu-interpreter -b apps/fft/build/hello-rvv-validate-n256-r1-idft0-riscv64-nemu.bin
```

实际 binary 名称随 `ARCH` 和参数变化，以 `build/` 中 Makefile 生成的名称为准。FFT 验证是 bit-exact：scalar、RVV、EXT 的每个 `int16_t` 实部/虚部必须与 reference 完全一致，输出必须出现 `CHECK: PASS`；不能使用 MIMO 式容差。

## XiangShan EMU 性能

性能构建使用 `FFT_VALIDATE=0`，最终指标只看 XiangShan EMU 的 `FFT_ROI` cycle。长任务必须 detached 运行：

```bash
mkdir -p /tmp/fft-emu
nohup setsid ${XS_ENV}/XiangShan/build/emu \
  --no-diff --force-dump-result \
  -i apps/fft/build/hello-ext-perf-n4096-r1-idft0-riscv64-xs.bin \
  > /tmp/fft-emu/ext-n4096.log 2>&1 < /dev/null &
echo $!
```

`${XS_ENV}` 指向已编译 XiangShan EMU 的环境目录。build、binary 和日志均为本地产物，不纳入 Git；不能把 NEMU 的运行时间或 whole-program 指标当作最终性能结论。
