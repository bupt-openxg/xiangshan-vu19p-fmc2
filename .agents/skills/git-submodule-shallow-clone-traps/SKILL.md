---
name: git-submodule-shallow-clone-traps
description: 用浅克隆/--depth 1 初始化 git submodule，或用脚本自动拉取多仓库依赖（含 submodule）时使用。覆盖：停在默认分支顶端、空工作树、foreach 的 dash 限制、递归范围最小化、生成物内嵌 gitDirty、离线镜像验证。触发词：submodule、--depth 1、浅克隆、git am、clone 失败、not found object。
---

# git submodule 浅拉取三类陷阱（均实测复现，附修复配方）

## 陷阱

1. **`--depth 1` 会把 submodule 停在默认分支顶端**，而非父仓库 gitlink 记录的 commit（实测一次 7/9 命中）。症状：后续构建报 `not found: object xxx` 或版本错配。GitHub 允许按 SHA 抓取可达提交（`git fetch --depth 1 origin <sha>`）。
2. **默认分支顶端恰好等于记录 SHA 时，`submodule--helper` 认为"已是目标状态"而跳过 checkout**，留下 **HEAD 正确但工作树为空**的目录（`git status` 显示所有文件 staged-deleted）。症状同 1，但 `git submodule status` 的前缀是空格、**看不出问题**。
3. **`git submodule foreach` 的内嵌脚本由 `/bin/sh`（Ubuntu 上是 dash）执行**：`${var:0:8}`、`<<<` 等 bash 语法报 `Bad substitution` 且整条 foreach 非零退出。只用 POSIX（`printf %.8s`）。
4. **范围最小化**：先读构建文件确认哪些 submodule 真是源码依赖（如 mill 的 `millSourcePath`、CMake 的 add_subdirectory 引用），只 `--init` 顶层 + 显式列出必需的嵌套；`--recursive` 常把几十 GB 的仿真/测试依赖拖进来并因网络失败中断（体积对照实测 156MB vs 1.7GB）。
5. **生成物可能内嵌 git 状态**：有的项目把 `gitDirty`/`git diff` 写进生成代码（香山即如此）→ 依赖树必须干净才能复现；嵌套 submodule 的 dirty 会逐层以 ` m` 上报到顶层。清嵌套污染：`git -C <sub> submodule deinit -f <nested>`（未初始化/空目录 = 上游原生状态，通常就是干净态）。

## 统一修复循环（可抄进脚本，注意是 POSIX sh）

```bash
verify_submodules() {   # 用法: verify_submodules <仓库路径>
  git -C "$1" submodule foreach --quiet '
    cur=$(git rev-parse HEAD 2>/dev/null || echo none)
    want=$(printf %.8s "$sha1")          # $sha1/$sm_path 由 git foreach 提供
    if [ "$cur" != "$sha1" ]; then
      echo "修复 $sm_path: $(printf %.8s "$cur") -> $want"
      git fetch --depth 1 origin "$sha1" && git reset --hard -q "$sha1"
    elif [ -z "$(ls -A . | grep -vx .git)" ]; then
      echo "修复 $sm_path: 空工作树 -> $want"
      git reset --hard -q "$sha1"
    fi
  '
}
```

要点：先判 SHA 不符（陷阱 1）再判空工作树（陷阱 2，`reset --hard` 一并清掉 staged-deleted 状态）；嵌套层（如 `rocket-chip/{cde,hardfloat}`）对子模块仓库再跑一次同一函数。

## 网络受限/离线时验证同一代码路径

用配置把远端 URL 映射到本地已有仓库（clone/am/submodule 全流程同路径验证）：

```bash
declare -x GIT_CONFIG_COUNT=N
# 逐个: GIT_CONFIG_KEY_i=url.file:///abs/path/to/local/repo.insteadOf
#        GIT_CONFIG_VALUE_i=https://github.com/OWNER/REPO.git
# 另加一条: protocol.file.allow=always    ← git>=2.38 对 submodule 的 file:// 默认禁止
```
`file://` 普通 fetch 抓"未广告 SHA"无需服务端配置；环境变量形式的 GIT_CONFIG_* 会被子进程（含 submodule 子命令）继承。

## 校验清单

- `git submodule status`：`-`=未初始化 / `+`=SHA 不符 / 空格=对齐；对构建关键路径抽查已知文件是否存在（如 `<sub>/build.sc`）。
- 顶层 `git status --short` 是否干净（发现 ` m <sub>` 就向下逐层钻）。
