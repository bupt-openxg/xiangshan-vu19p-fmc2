# MIMO 均衡冻结实现

这是 `xvbbp` 硬件分支对应的 MIMO 均衡软件包。这里只保留最终冻结的源码和编译输入；实验候选、历史源码、仿真日志、性能报告和质量数据不随本包发布。

## 唯一配置入口

一个配置由 `MIMO_KERNEL`（数学任务和点数）与 `MIMO_IMPL`（实现后端）共同确定。每个组合只有一个实现入口：

| Kernel | scalar | RVV | EXT+RVV |
|---|---|---|---|
| `eq_zf_2x2` | `kernels/eq_zf/scalar/eq_zf_scalar.c` | `kernels/eq_zf/rvv/eq_zf_rvv.c` | `kernels/eq_zf/ext/eq_zf_ext.c` |
| `eq_mmse_2x2` | `kernels/eq_mmse/scalar/eq_mmse_scalar.c` | `kernels/eq_mmse/rvv/eq_mmse_rvv.c` | `kernels/eq_mmse/ext/eq_mmse_ext.c` |
| `eq_zf_4x4` | scalar 入口 | RVV 入口 | EXT 入口 |
| `eq_mmse_4x4` | scalar 入口 | RVV 入口 | EXT 入口 |

最终定点口径、布局和后端专用编译宏保存在 `config/profiles/*.mk`。这些文件是构建输入，不是测试报告，不能在运行时替换为实验候选。

## 构建

先准备 Nexus-AM 和交叉编译器：

```bash
export AM_HOME=/path/to/nexus-am
export RVV_TOOLCHAIN_PREFIX=/opt/riscv/bin/riscv64-unknown-elf-
```

验证构建示例：

```bash
make -C apps/mimo \
  MIMO_IMPL=scalar \
  MIMO_VALIDATE_KERNEL=eq_zf_2x2 \
  MIMO_PERF=0
```

性能构建示例：

```bash
make -C apps/mimo \
  MIMO_IMPL=ext \
  MIMO_PERF=1 \
  MIMO_PERF_KERNEL=eq_mmse_4x4 \
  MIMO_WARMUP_REPEAT=1 \
  MIMO_REPEAT=1
```

一次编译全部 12 个冻结组合：

```bash
make -C apps/mimo xvbbp-build-all AM_HOME="$AM_HOME"
```

把 `MIMO_IMPL` 设为 `scalar`、`rvv` 或 `ext`，把 kernel 设为四个表中名称之一即可。Makefile 会强制加载对应冻结 profile；不支持 8x8、实验 profile 或事故 stub 路径。

## NEMU 正确性

构建完成后，使用生成的 binary 运行 NEMU：

```bash
${AM_HOME}/NEMU/build/riscv64-nemu-interpreter -b apps/mimo/build/mimo-scalar
```

三种后端必须使用相同的输入任务、Q15 数值合同和 `MIMO_VALIDATE_TOL=11` 口径；验证输出应报告通过，误差统计只能作为辅助信息。

## XiangShan EMU 性能

EMU 性能只看程序打印的 ROI cycle。官方运行必须脱离终端和 SSH 会话：

```bash
mkdir -p /tmp/mimo-emu
nohup setsid ${XS_ENV}/XiangShan/build/emu \
  --no-diff --force-dump-result \
  -i apps/mimo/build/mimo-ext \
  > /tmp/mimo-emu/eq_mmse_4x4-ext.log 2>&1 < /dev/null &
echo $!
```

其中 `${XS_ENV}` 指向包含已编译 XiangShan EMU 的环境目录。长时间任务必须使用这种 detached 离线方式；日志和 binary 放在 `/tmp` 或其他外部产物目录，不写入本仓库。

最终性能结论只使用 XiangShan EMU 的 ROI cycle；NEMU 只负责功能正确性筛选，不能替代 EMU 性能结论。
