# AM/NEMU 复杂测试上板记录（2026-10-20）

同一块 PHINE PD-VU19P-2-LSI SOLO v3、同一份 XVBBP 比特流
（`build/fpga_top_debug.bit`，WNS +0.709ns / WHS +0.010ns）。
负载来自 `third_party/nexus-am/apps/`，按本板 **16550 @ `0x310b0000`** 重新构建
（预编译的 `ready-to-run/*.bin` 默认走 uartlite，本板不可用）：

```bash
flows/11_build_am_bench.sh
flows/10_run_xvbbp.sh build/am-bench/dhrystone.bin
flows/10_run_xvbbp.sh build/am-bench/coremark-2iter.bin
flows/10_run_xvbbp.sh build/am-bench/microbench-test.bin
```

| 负载 | 关键结果 | 日志 |
|---|---|---|
| Dhrystone | `Dhrystone PASS`；500,000 runs；`cycle 127500620 inst 209500073 ipc 1.643`；**2.23 DMIPS/MHz** | `dhrystone.log` |
| CoreMark（2 次迭代）| 2K performance run；CRC 全匹配（`crclist 0xe714`、`crcmatrix 0x1fd7`、`crcstate 0x8e3a`、**`crcfinal 0x72be`**）；总时间 16.35s | `coremark-2iter.log` |
| MicroBench（test 规模）| qsort / queen / bf / fib / sieve / 15pz / dinic / lzip / ssort / md5 **10 项全 Passed**；`MicroBench PASS`；42s | `microbench-test.log` |

## 读数说明

- Dhrystone 的 `DMIPS/MHz` 由 `mcycle` 计数导出，与频率假设无关，**2.23** 可直接引用；
  日志里的 `Freq is set to 2.000000e+09 HZ` 是 AM 端写死的假设值，绝对
  `Dhrystones_Per_Second`/`DMIPS` 按它换算，不代表本板实际时钟。
- CoreMark 日志的 `CoreMark Iterations/Sec 122.23` 是 AM 计时单位换算的结果；
  按日志自身的 `Total time 16362 ms` 与 2 次迭代计算，实际为 **≈0.122 iter/s**。
  判定依据是 2K 标准 CRC 全部匹配，而非打印的速率。
- MicroBench 的 `Total time` 同样来自 AM 计时器，仅作相对参考；判定依据是 10 项
  子测试全部 `Passed.` 与 `MicroBench PASS`。
