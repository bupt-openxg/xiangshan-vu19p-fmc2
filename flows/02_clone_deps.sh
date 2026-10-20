#!/usr/bin/env bash
# =============================================================================
# 02 拉取第三方依赖并应用移植补丁（不使用 git submodule 递归）
#   third_party/XiangShan     上游香山 @0a150819（XVBBP 分支合并的上游父提交）
#                             + patches/xiangshan-xvbbp（5 文件核心补丁）
#                             + yunsuan 包固定到 GaryCAICHI/YunSuan@b4a62448（V 数据通路）
#   third_party/env-scripts   上游 main → 补丁基线 → git am patches/env-scripts/*.patch
#   third_party/nexus-am      GaryCAICHI/nexus-am@9dc5f1e1 + patches/nexus-am 0001-0003（UART 25MHz / VS / lp64d；0004 由 flows/09 用于 apps）
#   third_party/riscv-xvbbp   XVBBP 冻结软件包 @c553928（FFT/MIMO 工作负载源）
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true
TP="$ROOT/third_party"
mkdir -p "$TP"

XS_URL="https://github.com/OpenXiangShan/XiangShan.git"
XS_SHA="0a150819d1d1aca1f351975d455d3102cbc2bc24"
YS_URL="https://github.com/GaryCAICHI/YunSuan.git"
YS_BRANCH="kunminghu-v2-xvbbp"
YS_SHA="b4a62448b1539ab875daf36a62fe9cf6bc2fbe7b"
ES_URL="https://github.com/OpenXiangShan/env-scripts.git"
ES_BASE="876f39e4cf690745a3951ac6c06ce5475d7d4f16"
AM_URL="https://github.com/GaryCAICHI/nexus-am.git"
AM_SHA="9dc5f1e19b3e0dc58beade6f9a2e25dc90a6f2b8"
XVP_URL="https://github.com/bupt-openxg-lab/riscv-xvbbp.git"
XVP_SHA="c55392867c9d7c99c1677f49d6cfa0652a0e2be3"

# ---------- XiangShan（上游香山源码，生成 RTL 用） ----------
if [ -d "$TP/XiangShan/.git" ]; then
    echo "[02] XiangShan 已存在（HEAD $(git -C "$TP/XiangShan" rev-parse --short HEAD 2>/dev/null || echo none)），检查基线"
else
    echo "[02] 克隆 XiangShan @${XS_SHA:0:8}（浅拉取单 commit）"
    mkdir -p "$TP/XiangShan"
    git -C "$TP/XiangShan" init -q
    git -C "$TP/XiangShan" remote add origin "$XS_URL"
    git -C "$TP/XiangShan" fetch --depth 1 origin "$XS_SHA"
    git -C "$TP/XiangShan" checkout -q FETCH_HEAD
fi
# 允许 HEAD 是"基线 + 本地 xvbbp 提交"；必须是基线的后代
if ! git -C "$TP/XiangShan" merge-base --is-ancestor "$XS_SHA" HEAD 2>/dev/null; then
    echo "[02] ERROR: $TP/XiangShan 不是从 $XS_SHA 派生的（当前 $(git -C "$TP/XiangShan" rev-parse --short HEAD 2>/dev/null)）"
    echo "      本工程需要全新拉取：rm -rf third_party/XiangShan 后重跑 flows/02"
    exit 1
fi

# ---- submodule：顶层 11 个（build.mill 的源码依赖），嵌套 3 个 ----
#   yunsuan 的本地 URL 指向 XVBBP fork（含上游全部历史；不修改 .gitmodules，保持工作树干净）
git -C "$TP/XiangShan" config submodule.yunsuan.url "$YS_URL"
if git -C "$TP/XiangShan" submodule status | grep -q '^-'; then
    echo "[02] 初始化 XiangShan 顶层 submodule（--init --depth 1）"
    git -C "$TP/XiangShan" submodule update --init --depth 1
fi
if [ ! -d "$TP/XiangShan/rocket-chip/cde/cde" ] || [ ! -d "$TP/XiangShan/rocket-chip/hardfloat/hardfloat" ]; then
    echo "[02] 初始化 rocket-chip 嵌套 submodule（cde/hardfloat）"
    git -C "$TP/XiangShan/rocket-chip" submodule update --init --depth 1 cde hardfloat
fi
if [ ! -f "$TP/XiangShan/openLLC/openNCB/build.sc" ]; then
    echo "[02] 初始化 openLLC/openNCB 嵌套 submodule"
    git -C "$TP/XiangShan/openLLC" submodule update --init --depth 1 openNCB
fi

# 校验并修复（两种实测过的 submodule 陷阱都会让后续 RTL 编译报 "not found: object xxx"）:
#   a) --depth 1 抓取可能把 submodule 停在默认分支顶端而非父仓库 gitlink 记录的 commit;
#   b) 若默认分支顶端恰好等于记录 commit, submodule--helper 会跳过 checkout, 留下
#      "HEAD 正确但工作树为空" 的目录。
verify_submodules() {
    local repo="$1" label="$2"
    # 注意: foreach 的内嵌脚本由 /bin/sh (dash) 执行, 不能用 ${var:0:8} 等 bash 语法
    git -C "$repo" submodule foreach --quiet '
        cur=$(git rev-parse HEAD 2>/dev/null || echo none)
        want=$(printf %.8s "$sha1")
        if [ "$cur" != "$sha1" ]; then
            echo "  [修复] '"$label"'/$sm_path: $(printf %.8s "$cur") -> $want"
            git fetch --depth 1 origin "$sha1"
            git reset --hard -q "$sha1"
        elif [ -z "$(ls -A . | grep -vx .git)" ]; then
            echo "  [修复] '"$label"'/$sm_path: 工作树为空 -> 检出 $want"
            git reset --hard -q "$sha1"
        fi
    '
}
verify_submodules "$TP/XiangShan" "XiangShan"
verify_submodules "$TP/XiangShan/rocket-chip" "rocket-chip"
verify_submodules "$TP/XiangShan/openLLC" "openLLC"

# ---- YunSuan：固定到 XVBBP 分支的冻结 SHA（V 扩展的运算数据通路包） ----
YS_REPO="$TP/XiangShan/yunsuan"
if [ "$(git -C "$YS_REPO" rev-parse HEAD)" != "$YS_SHA" ]; then
    echo "[02] 固定 yunsuan -> GaryCAICHI/YunSuan@${YS_SHA:0:8}（$YS_BRANCH）"
    git -C "$YS_REPO" fetch -q --depth 1 "$YS_URL" "$YS_SHA" || {
        echo "[02] ERROR: 无法从 $YS_URL 获取 $YS_SHA"; exit 1; }
    git -C "$YS_REPO" checkout -q "$YS_SHA"
fi
[ "$(git -C "$YS_REPO" rev-parse HEAD)" = "$YS_SHA" ] || {
    echo "[02] ERROR: yunsuan SHA 不符: $(git -C "$YS_REPO" rev-parse HEAD)"; exit 1; }

# ---- XVBBP 核心侧补丁（5 个 Scala 文件）----
XPATCH="$ROOT/patches/xiangshan-xvbbp/0001-xvbbp-core.patch"
if git -C "$TP/XiangShan" apply --reverse --check "$XPATCH" >/dev/null 2>&1; then
    echo "[02] XVBBP 核心补丁已应用"
else
    echo "[02] 应用 XVBBP 核心补丁（5 个 Scala 文件）"
    git -C "$TP/XiangShan" apply --index "$XPATCH"
fi

# ---- 本地提交：补丁 + yunsuan gitlink，保证生成 RTL 时工作树干净（gitDirty=0）----
if ! git -C "$TP/XiangShan" symbolic-ref -q HEAD >/dev/null; then
    git -C "$TP/XiangShan" switch -c xvbbp-local
fi
git -C "$TP/XiangShan" add -A
if ! git -C "$TP/XiangShan" diff --cached --quiet; then
    git -C "$TP/XiangShan" -c user.name="ReConstruct" -c user.email="reconstruct@local" \
        commit -q -m "local: xvbbp core support (5-file patch) + yunsuan package pin"
fi
if [ -n "$(git -C "$TP/XiangShan" status --porcelain)" ]; then
    echo "[02] ERROR: XiangShan 工作树不干净（香山会把 dirty 写进 RTL）:"
    git -C "$TP/XiangShan" status --short | head -20
    exit 1
fi
echo "[02] XiangShan OK: 基线 ${XS_SHA:0:8} + 本地 $(git -C "$TP/XiangShan" rev-parse --short HEAD)，yunsuan ${YS_SHA:0:8}"

# ---------- nexus-am（FFT/MIMO 工作负载的运行时）+ 板级补丁 ----------
if [ -d "$TP/nexus-am/.git" ]; then
    echo "[02] nexus-am 已存在，检查基线"
else
    echo "[02] 克隆 nexus-am @${AM_SHA:0:8}"
    git clone -q --depth 1 "$AM_URL" "$TP/nexus-am"
fi
if [ "$(git -C "$TP/nexus-am" rev-parse HEAD)" != "$AM_SHA" ]; then
    git -C "$TP/nexus-am" fetch -q --depth 1 origin "$AM_SHA"
    git -C "$TP/nexus-am" checkout -q "$AM_SHA"
fi
[ "$(git -C "$TP/nexus-am" rev-parse HEAD)" = "$AM_SHA" ] || {
    echo "[02] ERROR: nexus-am SHA 不符"; exit 1; }
# 0001-0003 作用于 AM 源码树; 0004 作用于 apps/fft（由 flows/09 在拷贝后应用）
for p in "$ROOT"/patches/nexus-am/000[1-3]-*.patch; do
    [ -f "$p" ] || continue
    if git -C "$TP/nexus-am" apply --reverse --check "$p" >/dev/null 2>&1; then
        echo "[02] nexus-am 补丁已应用: $(basename "$p")"
    else
        echo "[02] 应用 nexus-am 补丁: $(basename "$p")"
        git -C "$TP/nexus-am" apply "$p"
    fi
done

# ---------- riscv-xvbbp（冻结的 FFT/MIMO 软件包，提供 apps 源码） ----------
if [ -d "$TP/riscv-xvbbp/.git" ]; then
    echo "[02] riscv-xvbbp 已存在，检查基线"
else
    echo "[02] 克隆 riscv-xvbbp @${XVP_SHA:0:8}"
    git clone -q --depth 1 "$XVP_URL" "$TP/riscv-xvbbp"
fi
if [ "$(git -C "$TP/riscv-xvbbp" rev-parse HEAD)" != "$XVP_SHA" ]; then
    git -C "$TP/riscv-xvbbp" fetch -q --depth 1 origin "$XVP_SHA"
    git -C "$TP/riscv-xvbbp" checkout -q "$XVP_SHA"
fi
[ "$(git -C "$TP/riscv-xvbbp" rev-parse HEAD)" = "$XVP_SHA" ] || {
    echo "[02] ERROR: riscv-xvbbp SHA 不符"; exit 1; }

# ---------- env-scripts（上游 FPGA 流程）+ 移植补丁 ----------
if [ -d "$TP/env-scripts/.git" ]; then
    echo "[02] env-scripts 已存在，跳过克隆"
else
    echo "[02] 克隆 env-scripts (main)"
    git clone -q "$ES_URL" "$TP/env-scripts"
fi
cd "$TP/env-scripts"
if [ -d .git/rebase-apply ]; then
    echo "[02] 检测到未完成的 git am，先 abort 重来"
    git am --abort || true
fi
if git log --format=%s -1 | grep -q '^phine:'; then
    echo "[02] env-scripts 移植补丁已应用，跳过"
else
    echo "[02] checkout 补丁基线 ${ES_BASE:0:8} 并应用 13 个补丁"
    git checkout -q "$ES_BASE"
    git -c user.name="ReConstruct" -c user.email="reconstruct@local" \
        am -q "$ROOT"/patches/env-scripts/*.patch
    echo "[02] 补丁应用完成: $(git log --oneline -1)"
fi

echo
echo "==== 依赖就绪 ===="
echo "  XiangShan   : $(git -C "$TP/XiangShan" rev-parse --short HEAD)（基线 ${XS_SHA:0:8} + xvbbp 补丁）"
echo "  YunSuan     : $(git -C "$TP/XiangShan/yunsuan" rev-parse --short HEAD)（XVBBP 包）"
echo "  nexus-am    : $(git -C "$TP/nexus-am" rev-parse --short HEAD)（+ 板级补丁）"
echo "  riscv-xvbbp : $(git -C "$TP/riscv-xvbbp" rev-parse --short HEAD)"
echo "  env-scripts : $(git -C "$TP/env-scripts" rev-parse --short HEAD)  ($TP/env-scripts)"
echo "下一步: flows/03_gen_rtl.sh（生成 RTL，约 30-60 分钟）"
