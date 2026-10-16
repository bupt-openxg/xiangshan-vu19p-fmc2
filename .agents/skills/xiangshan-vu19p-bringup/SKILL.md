---
name: xiangshan-vu19p-bringup
description: 在本仓库执行"从零构建"（依赖拉取 → RTL 生成 → 比特流）时使用。覆盖 flows/ 步骤编排、submodule 浅拉取陷阱与自动修复、RTL 生成的强制重跑与可复现性要点。触发词：构建、复现、flow、make verilog、RTL、bitstream、submodule、clone、fudian、gitDirty。
---

# 香山 VU19P 从零构建（本仓库视角）

## 不可变事实（动手前先确认上游是否有变）

- 香山 commit：`4b9ddb8a6311bff6f5e29cb3722d1f4236d66292`；env-scripts 基线：`876f39e4cf690745a3951ac6c06ce5475d7d4f16`，其上 `git am` 应用 `patches/env-scripts/` 的 12 个补丁。
- 器件 `xcvu19p-fsva3824-1-e`；Vivado 2024.2；板卡时钟（AEConsole）：GC02=80MHz(DDR 参考)、GC04=25MHz(CPU)、GC06=1MHz(RTC)，VADJ=1.2V。
- 内存地址：DDR 载入点 `0x80000000`、UART16550 `0x310B0000`、裸机栈顶 `0x81000000`。

## 流程编排（flows/00..08，幂等，失败即停）

```
00 宿主检查(Vivado/磁盘≥80G/swap≥32G)   01 工具链(全部本地化到 tools/, 无需 sudo)
02 拉依赖+应用补丁      03 RTL(20-40min)   04 比特流(~7h)   05 烧录
06 Hello                07 Linux           08 板卡诊断(~20min)
```

命令与预期输出见 `README.md` §二；排障见 `docs/run-guide.md`。跳过 7h 构建的办法：把现成的 `fpga_top_debug.bit/.ltx` 放进 `build/`。

## submodule 陷阱（flows/02 已自动处理；手工操作必须知道）

1. **只初始化构建必需的**：顶层 10 个 + `rocket-chip/{cde,hardfloat}` + `openLLC/openNCB`（依据 XiangShan `build.sc` 的 `millSourcePath` 引用）。上游 `--recursive` 会拖入几十 GB 仿真/测试依赖并常因网络中断。体积对照：精简 **156MB** vs 递归 **1.7GB**。
2. **`--depth 1` 会把 submodule 停在默认分支顶端**（≠ 父仓库 gitlink 记录的 SHA）。症状：`make verilog` 编译期报 `not found: object fudian` 一类错误。
3. **默认分支顶端恰好等于记录 SHA 时，submodule--helper 跳过 checkout，留下"HEAD 正确但工作树为空"的目录**（实测 fudian、aia 命中）。`git submodule status` 看不出（前缀是空格）。
4. flows/02 的 `verify_submodules()` 会自动修复 2、3 两类（按 `$sha1` fetch + `git reset --hard`；空工作树检测）——**改动它时保留修复循环**。
5. **`git submodule foreach` 的内嵌脚本由 /bin/sh(dash) 执行**：`${var:0:8}` 等 bash 语法会报 `Bad substitution` 并让整个 foreach 失败；用 `printf %.8s` 等 POSIX 写法。
6. 状态判读：`git submodule status` 行首 `-`=未初始化、`+`=SHA 不符、空格=对齐。构建关键点抽查：`difftest/build.sc`、`rocket-chip/cde/cde`、`rocket-chip/hardfloat/hardfloat`、`openLLC/openNCB/build.sc` 存在。

## RTL 生成

- `cd third_party/XiangShan && make verilog CONFIG=MinimalConfig JVM_XMX=20G`；产物 `build/rtl`（1579 文件）。flows/03 附带模拟器残留检查（`tools/checks/rtl_check.sh`，DPI 须被 `ifndef SYNTHESIS` 门控）与后处理（`tools/patch_rtl.sh`：ClockGate 透传 / URAM ram_style / 移除 DummyDPI 例化链）。
- **make 会因产物已存在而跳过重生成**：要强制重生成先 `rm -rf build/rtl`，否则 `patch_rtl.sh` 打在旧产物上（表现为"移除 0 处"）。
- RTL 会内嵌 `gitDirty`（`NewCSR.sv` 的 `wire gitDirty`）与 `git diff` 注释（`XSTop.sv`）：**构建前保证 third_party/XiangShan 树干净（含全部 submodule）**，否则产出带 dirty 标记。清嵌套污染：`git -C <sub> submodule deinit -f <nested>`（曾因递归试验把 coupledL2 嵌套拉脏，逐层 ` m` 上报到顶层）。
- 可复现性基线：干净树下重建的 RTL 与"上板验证过的原构建"逐字节比对 **1578/1579 一致**；唯一差异 `XSTop.fir` 是 FIRRTL 中间文件的源码定位器绝对路径字符串（345 万处，不参与综合）。
- mill 编译产物有缓存；`build.sc` 的 `gitStatus` 是 `Task.Input`（每次运行重算）；JVM 内存默认 `JVM_XMX=20G`。

## 网络受限时的本地镜像验证法

用 `insteadOf` 把 GitHub URL 映射到本地已有仓库，走**同一代码路径**（适用于 clone/am/submodule 全流程验证）：

```bash
declare -x GIT_CONFIG_COUNT=N
# 逐个: GIT_CONFIG_KEY_i=url.file:///abs/path/repo.insteadOf
#        GIT_CONFIG_VALUE_i=https://github.com/OWNER/REPO.git
# 另加一条: protocol.file.allow=always   ← git>=2.38 对 submodule 的 file:// 默认禁止
```
`file://` 抓未广告 SHA（`git fetch <url> <sha>`）无需服务端额外配置。
