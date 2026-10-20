# XVBBP EXT 复杂矩阵上板（2026-10-20）

同一块 PHINE PD-VU19P-2-LSI SOLO v3、同一份比特流
（`build/fpga_top_debug.bit`，WNS +0.709ns / WHS +0.010ns / 0 路由错误）。

与之前的代表集不同，本矩阵覆盖 **EXT 后端的完整数据通路**：

- FFT：8 个尺寸（16/64/128/256/512/1024/2048/4096）× **正向/逆向**，共 16 个镜像；
  真实 FFT 数据流使用 `vscmul`（34 处/镜像）、`vscaddrq`（6 处）、`vscaddrtq`（6 处），
  每个镜像启动时的 n16 demo 再逐条执行全部 7 条指令。
- MIMO：全部 4 个 kernel（ZF/MMSE × 2x2/4x4）。以 4x4 MMSE 镜像为例，反汇编统计到
  `vscmul` 36、`vscmulcj` 56、`vscmacc` 20、`vscmacccj` 52 处，覆盖长序列的
  共轭复数乘/乘累加；每个 kernel 15600 个 site、数万校验点。

## 结果：21/21 PASS

| 组 | 结果 |
|---|---|
| smoke（EXT n16 七指令自检） | `CHECK: PASS` |
| FFT EXT 16 个配置 | 每个均 `EXT instruction demo: PASS` + `CHECK: PASS` |
| MIMO EXT ZF 2x2 | `MIMO_VALIDATE_PASS`，fail=0，max_abs=9/11 |
| MIMO EXT MMSE 2x2 | `MIMO_VALIDATE_PASS`，fail=0，max_abs=7/11 |
| MIMO EXT ZF 4x4 | `MIMO_VALIDATE_PASS`，fail=0，max_abs=10/11 |
| MIMO EXT MMSE 4x4 | `MIMO_VALIDATE_PASS`，fail=0，max_abs=10/11 |

MIMO 细节（`count`/`checksum` 见各自日志）：

| kernel | count | checksum |
|---|---|---|
| eq_zf_2x2 | 31200 | -1876700399 |
| eq_mmse_2x2 | 31200 | -612146619 |
| eq_zf_4x4 | 62400 | -390981967 |
| eq_mmse_4x4 | 62400 | -972240707 |

## 复现

```bash
MATRIX=1 flows/09_build_xvbbp_sw.sh      # 构建 16 FFT + 4 MIMO + smoke
flows/05_program_fpga.sh
flows/12_run_xvbbp_matrix.sh             # 逐项上板运行，约 40 分钟
# 或 BUILD=1 flows/12_run_xvbbp_matrix.sh 一次完成构建+运行
```

`summary.tsv` 是脚本生成的逐项 PASS/FAIL；`logs/<key>.log` 是 21 份串口实录。
单次运行的抓包时长：FFT 40s、MIMO 90–120s；实际程序在抓包窗口内早已完成。
