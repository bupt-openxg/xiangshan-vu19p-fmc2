#!/usr/bin/env bash
# =============================================================================
# 02 拉取第三方依赖并应用移植补丁（不使用 git submodule）
#   third_party/XiangShan     香山源码 @4b9ddb8（浅拉取单个 commit，约数百 MB）
#   third_party/env-scripts   上游 main → checkout 补丁基线 → git am patches/env-scripts/*.patch
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true
TP="$ROOT/third_party"
mkdir -p "$TP"

XS_URL="https://github.com/OpenXiangShan/XiangShan.git"
XS_SHA="4b9ddb8a6311bff6f5e29cb3722d1f4236d66292"
ES_URL="https://github.com/OpenXiangShan/env-scripts.git"
ES_BASE="876f39e4cf690745a3951ac6c06ce5475d7d4f16"

# ---------- XiangShan（香山源码，生成 RTL 用） ----------
if [ -d "$TP/XiangShan/.git" ]; then
    echo "[02] XiangShan 已存在，跳过克隆"
else
    echo "[02] 克隆 XiangShan @${XS_SHA:0:8}（浅拉取单 commit）"
    mkdir -p "$TP/XiangShan"
    git -C "$TP/XiangShan" init -q
    git -C "$TP/XiangShan" remote add origin "$XS_URL"
    git -C "$TP/XiangShan" fetch --depth 1 origin "$XS_SHA"
    git -C "$TP/XiangShan" checkout -q FETCH_HEAD
fi
got=$(git -C "$TP/XiangShan" rev-parse HEAD)
[ "$got" = "$XS_SHA" ] || { echo "[02] ERROR: XiangShan commit 不符: $got"; exit 1; }
# ---- 上游 submodule：只初始化 RTL 构建（make verilog / build.sc）真正需要的部分 ----
# 顶层 9 个（rocket-chip/difftest/huancun/fudian/utility/yunsuan/coupledL2/openLLC/src/main/resources/aia）
# 是 build.sc 的 mill 源码依赖，全部必需。
# 嵌套层只有 3 个被 build.sc 直接引用为源码目录（其余都是仿真/测试用，几十 GB 且常因网络失败）：
#   rocket-chip/cde, rocket-chip/hardfloat  ->  build.sc:22-24,94-108
#   openLLC/openNCB                          ->  build.sc:150-152
needs_sm=0
[ -f "$TP/XiangShan/difftest/build.sc" ]             || needs_sm=1
[ -d "$TP/XiangShan/rocket-chip/cde/cde" ]           || needs_sm=1
[ -d "$TP/XiangShan/rocket-chip/hardfloat/hardfloat" ] || needs_sm=1
[ -f "$TP/XiangShan/openLLC/openNCB/build.sc" ]      || needs_sm=1
if [ "$needs_sm" = 1 ]; then
    echo "[02] 初始化 XiangShan 顶层 submodule（--init --depth 1）"
    git -C "$TP/XiangShan" submodule update --init --depth 1
    echo "[02] 初始化 3 个被 build.sc 引用的嵌套 submodule"
    git -C "$TP/XiangShan/rocket-chip" submodule update --init --depth 1 cde hardfloat
    git -C "$TP/XiangShan/openLLC"    submodule update --init --depth 1 openNCB
fi
# 校验并修复（两种实测过的 submodule 陷阱都会让后续 RTL 编译报 "not found: object xxx"）:
#   a) --depth 1 抓取可能把 submodule 停在默认分支顶端而非父仓库 gitlink 记录的 commit;
#   b) 若默认分支顶端恰好等于记录 commit, submodule--helper 会跳过 checkout, 留下
#      "HEAD 正确但工作树为空" 的目录（实测 fudian/aia 命中）。
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
echo "[02] XiangShan OK: ${got:0:8}（submodule 已就绪且 SHA 对齐）"

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
    echo "[02] checkout 补丁基线 ${ES_BASE:0:8} 并应用 12 个补丁"
    git checkout -q "$ES_BASE"
    git -c user.name="ReConstruct" -c user.email="reconstruct@local" \
        am -q "$ROOT"/patches/env-scripts/*.patch
    echo "[02] 补丁应用完成: $(git log --oneline -1)"
fi

echo
echo "==== 依赖就绪 ===="
echo "  XiangShan   : $(git -C "$TP/XiangShan" rev-parse --short HEAD)  ($TP/XiangShan)"
echo "  env-scripts : $(git -C "$TP/env-scripts" rev-parse --short HEAD)  ($TP/env-scripts)"
echo "下一步: flows/03_gen_rtl.sh（生成 RTL，约 20-40 分钟）"
