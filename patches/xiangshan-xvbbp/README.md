# XVBBP 核心侧补丁（OpenXiangShan/XiangShan）

## 是什么

`0001-xvbbp-core.patch` 把 XVBBP 的 7 条自定义向量指令接到**上游香山**的译码/异常/发射链路上。
它只改 5 个 Scala 文件（约 50 行）：

| 文件 | 改动 |
|---|---|
| `backend/decode/Instructions.scala` | 新增 `object Zvbaseband`：7 条 `BitPat` |
| `backend/decode/VecDecoder.scala` | 7 条指令 → `FuType.vimac`（复乘/乘累加）或 `FuType.vialuF`（复数加/RQ/RTQ）|
| `backend/decode/VecExceptionGen.scala` | `zvbasebandEewIllegal`：这 7 条必须 SEW=16 |
| `backend/decode/DecodeUnit.scala` | `vscmacc/vscmacccj` 加入 `vmaInsts`（向量乘累加语义）|
| `backend/fu/wrapper/VIMacU.scala` | 把 `isComp/isConj` 接到 VIMac 数据通路 |

真正的复乘/共轭/饱和运算在 **YunSuan 包**里（不是本补丁）：见 `flows/02_clone_deps.sh`
把 `third_party/XiangShan/yunsuan` 固定到 `GaryCAICHI/YunSuan@b4a62448` 的原因与说明
（`docs/xvbbp.md`）。

## 基线与来源

- **基线**：上游 `OpenXiangShan/XiangShan@0a150819d1d1aca1f351975d455d3102cbc2bc24`
  （2026-01-23；正是 XVBBP 分支合并进的上游父提交，译码 API 原生匹配，无需回移旧 V2）
- **来源**：`GaryCAICHI/XiangShan` 分支 `kunminghu-v2-xvbbp` 的冻结提交
  `e34d8adbaf7cd890a7b148c1e9622d38435996b1`（该分支的 XVBBP 提交链：
  `a5f7722`、`2f296cb`、`2636207`、`2569195`、`89ff62d`（merge）、`2aeec70`、`e67a1ab`、`e34d8adb`）
- **生成命令**（在临时 clone 中执行；两处 fetch 也可改用 SSH）：

```bash
git init /tmp/xvbbp-patchgen && cd /tmp/xvbbp-patchgen
git fetch --depth 1 https://github.com/OpenXiangShan/XiangShan.git 0a150819d1d1aca1f351975d455d3102cbc2bc24
git fetch --depth 1 https://github.com/GaryCAICHI/XiangShan.git e34d8adbaf7cd890a7b148c1e9622d38435996b1
git diff 0a150819 e34d8adb -- \
  src/main/scala/xiangshan/backend/decode/DecodeUnit.scala \
  src/main/scala/xiangshan/backend/decode/Instructions.scala \
  src/main/scala/xiangshan/backend/decode/VecDecoder.scala \
  src/main/scala/xiangshan/backend/decode/VecExceptionGen.scala \
  src/main/scala/xiangshan/backend/fu/wrapper/VIMacU.scala
```

`git diff --stat 0a150819 e34d8adb` 的全部改动为 7 个路径（上述 5 个 Scala 文件 +
`.gitmodules` + `yunsuan` gitlink）；本补丁**只保留 5 个 Scala 文件**，`yunsuan` 指针由
flows/02 显式固定（见上），`.gitmodules` 使用上游原样。

## 应用方式

`flows/02_clone_deps.sh` 自动完成：

```bash
git -C third_party/XiangShan apply --index patches/xiangshan-xvbbp/0001-xvbbp-core.patch
git -C third_party/XiangShan add -A        # 同时记录 yunsuan 的 fork SHA
git -C third_party/XiangShan commit -m "local: xvbbp core support + yunsuan package pin"
```

**必须在香山仓库里落成一个本地提交**：香山会把 `git status` 的 dirty 状态写进生成 RTL，
工作树不干净会导致 `gitDirty=1` 且无法复现。

## 许可

补丁内容源自 XVBBP 集成工作（Cai Chi 等，见 `bupt-openxg-lab/riscv-xvbbp` 的 `NOTICE`），
上游 XiangShan / YunSuan 源码采用 Mulan Permissive Software License v2；本补丁为其最小化衍生物，
仅用于把该扩展集成到本仓库的上游基线中。
