# 离线构建交接：本机 → 无 GitHub 的大机器

2026-10-20 的实际做法。适用于新机器不能访问 GitHub、也不想重跑综合+布局的场景。
大机器上的 Vivado 必须与 DCP 同版本（本仓库为 **2024.2**）。

## 交接包（本机 `build/handoff/`，约 1.3GB，不入库）

| 文件 | 说明 |
|---|---|
| `fpga_top_debug_physopt.dcp` | **快速路径必需**：post-place phys_opt 检查点（622MB）|
| `fpga_top_debug_placed.dcp` | 可选回退（布局完成，622MB）|
| `resume_impl_route.tcl` | 从 physopt 续跑布线→报告→比特流（同 `tools/resume_impl_route.tcl`）|
| `xvbbp-all.bundle` | 仓库全部分支的 git bundle（204KB）|
| `rtl.tar.zst` | 已生成的 RTL（1821 文件，26MB），回退路径用 |
| `env-scripts-src.tar.zst` | 已打 13 个补丁的 env-scripts 源码（无 Vivado 工程，14MB）|
| `images/` | 7 个 FFT/MIMO 裸机镜像 + `images.tsv` |
| `*.rpt` | 中断前的布线时序/路由/DRC 报告 |
| `SHA256SUMS` | 传输后先校验 |

生成方式：`git bundle create build/handoff/xvbbp-all.bundle --all`；
RTL 用 `tar --use-compress-program='zstd -d'` 打包；其余直接复制。

## 传输

```bash
rsync -a --partial --info=progress2 -e "ssh -p <port>" \
      build/handoff/ <user>@<host>:<target>/handoff/
ssh -p <port> <user>@<host> 'cd <target>/handoff && sha256sum -c SHA256SUMS'
```

## 快速路径（约 50 分钟，Vivado 2024.2）

```bash
source <Vivado>/settings64.sh
export XILINXD_LICENSE_FILE=<license.lic>
cd <target>/handoff && mkdir -p out
nohup setsid vivado -mode batch -source resume_impl_route.tcl \
  -tclargs "$PWD/fpga_top_debug_physopt.dcp" "$PWD/out" "$PWD/out" \
  > resume.log 2>&1 < /dev/null &
tail -f resume.log        # 最后一行 === RESUME DONE ===
```

产物：`out/fpga_top_debug.bit`（约 190MB）、`out/fpga_top_debug.ltx`、
`out/fpga_top_debug_routed.dcp`、时序/路由/DRC/利用率报告。
本仓库的实测结果：WNS **+0.709ns** / WHS **+0.010ns** / TNS=THS=0 / 0 路由错误。

## 回退路径（DCP 不兼容或需要重跑综合）

```bash
git clone xvbbp-all.bundle ReConstruct && cd ReConstruct && git switch xvbbp
mkdir -p third_party/XiangShan/build
tar --use-compress-program='zstd -d' -xf ../rtl.tar.zst -C third_party/XiangShan/build
tar --use-compress-program='zstd -d' -xf ../env-scripts-src.tar.zst -C third_party
source tools/env.sh
cd third_party/env-scripts/fpga_diff
make project CPU=kmh NO_DIFF=1 DDR_RANK_WIDTH=1 CORE_DIR="$(cd ../../../XiangShan/build && pwd)"
VIVADO_JOBS=8 bash ../../../tools/run_bitstream.sh
```

## 回传与验证

```bash
rsync -a <user>@<host>:<target>/handoff/out/fpga_top_debug.bit \
          <user>@<host>:<target>/handoff/out/fpga_top_debug.ltx build/
flows/05_program_fpga.sh
flows/06_run_hello.sh
flows/10_run_xvbbp.sh smoke
```

注意：快速路径的 `open_checkpoint` 会打印 `Loading part xcvu19p-fsva3824-1-e`，
与 DCP 一致即可；license 在 `route_design`/`write_bitstream` 阶段生效。
