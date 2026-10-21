# tests/ —— BUPT XVBBP 负载上板测试

本目录只保留 `bupt-openxg/riscv-xvbbp` 仓库中出现的测试负载：

- **FFT 48 个镜像**：3 个后端（`scalar` / `rvv` / `ext`）× 8 个点数
  （16/32/64/128/256/512/1024/4096）× 2 个方向（正变换/逆变换）。
- **MIMO 12 个镜像**：3 个后端 × 4 个内核（2×2 ZF、2×2 MMSE、4×4 ZF、4×4 MMSE）。
- 其中 `ext` 后端的 FFT n16 镜像同时作为 7 条 XVBBP 指令的冒烟测试
  （打印 `EXT instruction demo: PASS`）。

测试范围与 BUPT 仓库保持一致。负载源码不复制进本仓库，构建时从
`third_party/riscv-xvbbp/software/nexus-am/apps` 临时安装到官方 nexus-am。

## 构建

```bash
tests/bupt/build.sh
```

- 依赖 `third_party/nexus-am`（官方 `086a238` + 板级 overlay）和
  `third_party/riscv-xvbbp`（`c553928`）。
- 统一 `CPPFLAGS=-DUART16550=1`（本板 16550 @ `0x310b0000`）。
- FFT 的 `oai_dfts.o` 单独用 `-march=rv64gc`：SIMDe 的 SSE `dft16` 在
  `-march=rv64gcv` 下映射到 RVV，本核重复调用会卡死；标量参考结果逐位不变。
- 产物：`build/bupt-<date>/images/{fft,mimo}` 和 `images.tsv`。
  默认日期目录可用 `BUPT_OUT=` 覆盖。

## 上板运行

前置：新比特流已烧录；`FPGA_BIT_HOME` 指向含 `fpga_top_debug.ltx` 的目录；
串口默认 `/dev/ttyUSB0`，115200 8N1。

```bash
export BIT_DIR=/path/to/bitstream
export FPGA_BIT_HOME="$BIT_DIR"
tests/bupt/run.sh
```

每个镜像由 `tests/common/board_run.sh` 在一次 Vivado 会话中完成
`halt → JTAG 写 DDR → VIO 复位`，同时抓串口，检测到 `Exit with code =`
提前结束。判定：

- FFT：`CHECK: PASS`；`ext` 镜像还必须 `EXT instruction demo: PASS`。
- MIMO：`MIMO_VALIDATE_PASS` 且 `MIMO_DONE status=0`。

输出：`build/bupt-<date>/logs/<key>.log`、`summary.tsv`，最后一行
`BUPT_RESULT total=60 pass=.. fail=..`。

## DDR 检查

```bash
vivado -mode batch -source tests/bupt/ddr16_check.tcl \
    -tclargs "$BIT_DIR/fpga_top_debug.ltx"
```

在 2/6/10/14GB 和 16GB 末尾 8 字节分别写入不同 pattern 再读回，
全对打印 `DDR16G_PASS`。AXI 地址 34 位，合法范围 `0x0..0x3FFFFFFFF`。

## 结果

见仓库根 `README.md` 第 5 节“测试内容与结果”。
