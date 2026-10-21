# 课题二测试方案（任务一）FPGA 证据 — 2026-10-21

板卡：PHINE PD-VU19P-2-LSI SOLO v3；比特流 SHA-256
`89ea097b52c41d27ae9cc8beff9161948637953e09d606b9f09a3040dc2a2589`；
测试源码版本见 `tests/upstream.env`。

| 文件 | 内容 |
|---|---|
| `ext-summary.log` / `ext-run.log` | 整核给定激励 52/52 |
| `random-summary.log` / `random-key.log` | 7 条扩展指令随机激励 10^7 条 |
| `fft-functional-summary.log` / `fft-functional-cases.tsv` | FFT 功能 140/140 |
| `fft-performance-summary.log` / `fft-performance-raw.tsv` | FFT 性能 14/14 ≥30% |
| `mimo-functional-summary.log` / `mimo-functional-cases.tsv` | MIMO 功能 16/16 |
| `mimo-performance-summary.log` / `mimo-performance-raw.tsv` | MIMO 性能：4x4 达标，2x2 未达标 |

复现命令见 `tests/README.md`；结果解读与差异说明见
`PPT/复现/课题二测试方案任务一_复现报告_20261021.docx`。
