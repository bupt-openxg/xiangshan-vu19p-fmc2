#!/usr/bin/env bash
# =============================================================================
# 02 拉取第三方依赖（全部官方仓库 + BUPT overlay，不使用 git submodule 递归）
#   third_party/XiangShan     OpenXiangShan/XiangShan@0a150819（官方）
#                             + bupt-openxg/riscv-xvbbp hardware/xiangshan/overlay（4 文件）
#                             + overlays/local/xiangshan/VIMacU.scala（2 行 isComp/isConj glue）
#   yunsuan                   OpenXiangShan/YunSuan@996b51d（官方）
#                             + bupt-openxg/riscv-xvbbp hardware/yunsuan/overlay（10 文件）
#   third_party/nexus-am      OpenXiangShan/nexus-am@086a238 + overlays/board/nexus-am
#   third_party/env-scripts   OpenXiangShan/env-scripts + patches/env-scripts（板卡移植）
#                             + overlays/board/env-scripts（16GB DDR）
#   third_party/riscv-xvbbp   bupt-openxg/riscv-xvbbp@c553928（唯一的非官方库；FFT/MIMO 源）
# =============================================================================
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/tools/env.sh" >/dev/null 2>&1 || true
TP="$ROOT/third_party"
mkdir -p "$TP"

XS_URL="https://github.com/OpenXiangShan/XiangShan.git"
XS_SHA="0a150819d1d1aca1f351975d455d3102cbc2bc24"
YS_URL="https://github.com/OpenXiangShan/YunSuan.git"
YS_SHA="996b51d86a7df502163a9e89ea3e23ff4b6cc4f9"
ES_URL="https://github.com/OpenXiangShan/env-scripts.git"
ES_BASE="876f39e4cf690745a3951ac6c06ce5475d7d4f16"
AM_URL="https://github.com/OpenXiangShan/nexus-am.git"
AM_SHA="086a238062fdfb3281b745a537b361983d0154b5"
XVP_URL="https://github.com/bupt-openxg/riscv-xvbbp.git"
XVP_SHA="c55392867c9d7c99c1677f49d6cfa0652a0e2be3"

# ---------- XiangShan（官方基线） ----------
if [ -d "$TP/XiangShan/.git" ]; then
    echo "[02] XiangShan 已存在（HEAD $(git -C "$TP/XiangShan" rev-parse --short HEAD 2>/dev/null || echo none)）"
else
    echo "[02] 克隆 XiangShan @${XS_SHA:0:8}（浅拉取单 commit）"
    mkdir -p "$TP/XiangShan"
    git -C "$TP/XiangShan" init -q
    git -C "$TP/XiangShan" remote add origin "$XS_URL"
    git -C "$TP/XiangShan" fetch --depth 1 origin "$XS_SHA"
    git -C "$TP/XiangShan" checkout -q FETCH_HEAD
fi
if ! git -C "$TP/XiangShan" merge-base --is-ancestor "$XS_SHA" HEAD 2>/dev/null; then
    echo "[02] ERROR: $TP/XiangShan 不是从官方 $XS_SHA 派生的"
    echo "      请 rm -rf third_party/XiangShan 后重跑 flows/02"
    exit 1
fi

# ---- 官方 submodule（含 yunsuan）；不修改 .gitmodules ----
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

# 修复浅拉取可能留下的错误 SHA / 空工作树（否则 RTL 编译报 "not found: object"）
verify_submodules() {
    local repo="$1" label="$2"
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

# ---- YunSuan：官方 SHA（不是 GaryCAICHI fork） ----
YS_REPO="$TP/XiangShan/yunsuan"
if [ "$(git -C "$YS_REPO" rev-parse HEAD)" != "$YS_SHA" ]; then
    echo "[02] 固定 yunsuan -> OpenXiangShan/YunSuan@${YS_SHA:0:8}"
    git -C "$YS_REPO" fetch -q --depth 1 "$YS_URL" "$YS_SHA"
    git -C "$YS_REPO" checkout -q "$YS_SHA"
fi
[ "$(git -C "$YS_REPO" rev-parse HEAD)" = "$YS_SHA" ] || {
    echo "[02] ERROR: yunsuan SHA 不符"; exit 1; }

# ---------- riscv-xvbbp（唯一的非官方库；FFT/MIMO 源） ----------
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

# ---- BUPT overlay（overlay 模式，不生成/应用 patch） ----
echo "[02] 应用 bupt-openxg hardware overlay"
cp -a "$TP/riscv-xvbbp/hardware/xiangshan/overlay/." "$TP/XiangShan/"
cp -a "$TP/riscv-xvbbp/hardware/yunsuan/overlay/." "$YS_REPO/"
cp -a "$ROOT/overlays/local/xiangshan/VIMacU.scala" \
      "$TP/XiangShan/src/main/scala/xiangshan/backend/fu/wrapper/VIMacU.scala"

# yunsuan 先提交，父仓库才能记录新的 gitlink
if ! git -C "$YS_REPO" symbolic-ref -q HEAD >/dev/null; then
    git -C "$YS_REPO" switch -C xvbbp-overlay
fi
git -C "$YS_REPO" add -A
if ! git -C "$YS_REPO" diff --cached --quiet; then
    git -C "$YS_REPO" -c user.name="ReConstruct" -c user.email="reconstruct@local" \
        commit -q -m "bupt-openxg overlay (XVBBP execution units)"
fi
[ -z "$(git -C "$YS_REPO" status --porcelain)" ] || {
    echo "[02] ERROR: yunsuan 工作树不干净"; exit 1; }

if ! git -C "$TP/XiangShan" symbolic-ref -q HEAD >/dev/null; then
    git -C "$TP/XiangShan" switch -C xvbbp-overlay
fi
git -C "$TP/XiangShan" add -A
if ! git -C "$TP/XiangShan" diff --cached --quiet; then
    git -C "$TP/XiangShan" -c user.name="ReConstruct" -c user.email="reconstruct@local" \
        commit -q -m "bupt-openxg overlay + VIMacU isComp/isConj glue on official ${XS_SHA:0:8}"
fi
if [ -n "$(git -C "$TP/XiangShan" status --porcelain)" ]; then
    echo "[02] ERROR: XiangShan 工作树不干净（香山会把 dirty 写进 RTL）:"
    git -C "$TP/XiangShan" status --short | head -20
    exit 1
fi
# 只允许 5 个源文件 + yunsuan gitlink 与官方不同
changed=$(git -C "$TP/XiangShan" diff --name-only "$XS_SHA" HEAD | grep -v '^yunsuan$' | wc -l | tr -d ' ')
[ "$changed" -eq 5 ] || {
    echo "[02] ERROR: XiangShan 与官方的差异不是 5 个源文件（实际 $changed）"; exit 1; }
echo "[02] XiangShan OK: 官方 ${XS_SHA:0:8} + BUPT overlay，yunsuan $(git -C "$YS_REPO" rev-parse --short HEAD)"

# ---------- nexus-am（官方 086a238 + 板级 overlay） ----------
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
echo "[02] 应用板级 nexus-am overlay"
cp -a "$ROOT/overlays/board/nexus-am/riscv64.mk" \
      "$TP/nexus-am/am/arch/isa/riscv64.mk"
cp -a "$ROOT/overlays/board/nexus-am/start.S" \
      "$TP/nexus-am/am/src/nemu/isa/riscv/boot/start.S"
cp -a "$ROOT/overlays/board/nexus-am/serial-16550.c" \
      "$TP/nexus-am/am/src/noop/common/serial-16550.c"

# ---------- env-scripts（官方流程 + 板卡移植 + 16GB DDR overlay） ----------
if [ -d "$TP/env-scripts/.git" ]; then
    echo "[02] env-scripts 已存在，检查基线"
else
    echo "[02] 克隆 env-scripts"
    git clone -q "$ES_URL" "$TP/env-scripts"
fi
cd "$TP/env-scripts"
if [ -d .git/rebase-apply ]; then
    echo "[02] 检测到未完成的 git am，先 abort"
    git am --abort || true
fi
if git log --format=%s -1 | grep -q '^phine:'; then
    echo "[02] env-scripts 移植补丁已应用"
else
    echo "[02] checkout 补丁基线 ${ES_BASE:0:8} 并应用 13 个补丁"
    git fetch -q origin "$ES_BASE"
    git checkout -q "$ES_BASE"
    git -c user.name="ReConstruct" -c user.email="reconstruct@local" \
        am -q "$ROOT"/patches/env-scripts/*.patch
fi
echo "[02] 应用 16GB DDR / JTAG / 30MHz CPU overlay"
cp -a "$ROOT/overlays/board/env-scripts/fpga_diff/vivado/tcl/common/xs_uart.tcl" \
      "$TP/env-scripts/fpga_diff/vivado/tcl/common/xs_uart.tcl"
cp -a "$ROOT/overlays/board/env-scripts/fpga_diff/vivado/tcl/common/jtag_ddr_subsys.tcl" \
      "$TP/env-scripts/fpga_diff/vivado/tcl/common/jtag_ddr_subsys.tcl"
cp -a "$ROOT/overlays/board/env-scripts/fpga_diff/src/rtl/common/fpga_top_debug.sv" \
      "$TP/env-scripts/fpga_diff/src/rtl/common/fpga_top_debug.sv"
cd "$ROOT"

echo
echo "==== 依赖就绪 ===="
echo "  XiangShan   : $(git -C "$TP/XiangShan" rev-parse --short HEAD)（官方 ${XS_SHA:0:8} + BUPT overlay）"
echo "  YunSuan     : $(git -C "$TP/XiangShan/yunsuan" rev-parse --short HEAD)（官方 ${YS_SHA:0:8} + BUPT overlay）"
echo "  nexus-am    : $(git -C "$TP/nexus-am" rev-parse --short HEAD)（官方 + 板级 overlay）"
echo "  riscv-xvbbp : $(git -C "$TP/riscv-xvbbp" rev-parse --short HEAD)"
echo "  env-scripts : $(git -C "$TP/env-scripts" rev-parse --short HEAD)（+ 16GB DDR）"
echo "下一步: flows/03_gen_rtl.sh（生成 RTL，约 30-60 分钟）"
