# XVBBP 上板验证证据（2026-10-20）

板卡：PHINE PD-VU19P-2-LSI SOLO v3（`xcvu19p-fsva3824-1-e`），VIO-only 控制，
UART `/dev/ttyUSB0` 115200 8N1。

## 比特流

- 来源：上游香山 `0a150819` + `patches/xiangshan-xvbbp/0001` + YunSuan `b4a62448`，
  `MinimalConfig`，Vivado 2024.2。
- 布线从 `fpga_top_debug_physopt.dcp` 续跑（`tools/resume_impl_route.tcl`，
  `route_design -directive Explore`），构建机 144 核 / 125GB。
- 时序（`fpga_top_debug_timing_summary_routed.rpt`）：
  **WNS +0.709ns / TNS 0 / WHS +0.010ns / THS 0**，全部约束满足，0 路由错误。
- SHA256 见 `bitstream.sha256`（`fpga_top_debug.bit` 190MB，不入库）。

## 运行结果

| 负载 | 关键输出 | 日志 |
|---|---|---|
| 裸机 Hello 回归 | `Hello, XiangShan! (MinimalConfig on PHINE PD-VU19P, 115200 8N1)` | `hello-serial.log` |
| smoke（官方 FFT EXT n16） | 7 条指令逐条 PASS + `EXT instruction demo: PASS` + `CHECK: PASS` + exit 0 | `smoke.log` |
| FFT EXT n1024 | `EXT instruction demo: PASS` + `CHECK: PASS` + exit 0 | `fft-ext-1024.log` |
| FFT EXT n4096 | `CHECK: PASS` + exit 0 | `fft-ext-4096.log` |
| FFT scalar n1024（基线） | `CHECK: PASS` + exit 0 | `fft-scalar-1024.log` |
| MIMO EXT ZF 4x4 | `MIMO_VALIDATE_PASS ... fail=0 max_abs=10 tol=11` + `MIMO_DONE status=0` | `mimo-ext-zf4.log` |
| MIMO EXT MMSE 2x2 | `MIMO_VALIDATE_PASS ... fail=0 max_abs=7 tol=11` + `MIMO_DONE status=0` | `mimo-ext-mmse2.log` |
| MIMO scalar ZF 4x4（基线） | `MIMO_VALIDATE_PASS ... fail=0` + `MIMO_DONE status=0` | `mimo-scalar-zf4.log` |

## 复现

```bash
flows/05_program_fpga.sh
flows/06_run_hello.sh
flows/09_build_xvbbp_sw.sh
flows/10_run_xvbbp.sh smoke
flows/10_run_xvbbp.sh fft-ext-1024
flows/10_run_xvbbp.sh fft-ext-4096
flows/10_run_xvbbp.sh mimo-ext-zf4
```

镜像构建使用 xPack `riscv-none-elf-gcc 15.2.0`，并带
`CPPFLAGS=-DUART16550=1`（本板 16550 @ `0x310b0000`；默认 uartlite
@ `0x40600000` 会让 CPU 卡死）。FFT 的 OAI 校验参考以 RV64GC 编译
（`patches/nexus-am/0004`）：SIMDe 的 RVV dft16 在本核上重复调用会卡死，
标量参考与 EXT/RVV 内核逐位一致。
