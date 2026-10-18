#!/usr/bin/env bash
# 02 mill (版本 = XiangShan/.mill-version) -> tools/mill/
#   两种分发形态（实测）:
#   - 旧版（≤0.12.16）: Maven 提供自包含 mill-dist-<ver>.jar（无 Main-Class），
#     由本脚本生成的 tools/mill/mill 包装脚本调用 mill.runner.client.MillClientMain。
#   - 新版（0.12.17+）: Maven 只有 mill-dist-<ver>-mill.sh 官方启动器（另附 .exe/.bat），
#     启动器按当前目录的 .mill-version 自行下载/缓存对应版本到 ~/.cache/mill/download。
#   注: tools/mill/ 整个目录被 gitignore, 全新克隆时必须由本脚本创建。
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VER="$(cat "$ROOT/third_party/XiangShan/.mill-version" 2>/dev/null || echo 0.12.3)"
MILL_DIR="$ROOT/tools/mill"
mkdir -p "$MILL_DIR"
BASE="https://repo1.maven.org/maven2/com/lihaoyi/mill-dist/${VER}"

JAR="$MILL_DIR/mill-dist-${VER}.jar"
if [ ! -f "$JAR" ]; then
    if curl -fsSL -o "$JAR.tmp" "${BASE}/mill-dist-${VER}.jar"; then
        mv "$JAR.tmp" "$JAR"
    else
        rm -f "$JAR.tmp"
    fi
fi

if [ -f "$JAR" ]; then
    # 旧版: 生成包装脚本（每次重建内容, 避免切换版本后残留旧启动方式）
    cat > "$MILL_DIR/mill" <<WRAPPER
#!/usr/bin/env bash
# mill launcher（由 tools/setup/02-mill.sh 生成）: 运行 mill-dist 自执行 jar
# 用法: mill [mill 参数...]   —— 需要 JAVA_HOME 已指向 JDK（本仓库: source tools/env.sh）
set -euo pipefail
exec java -cp "$JAR" mill.runner.client.MillClientMain "\$@"
WRAPPER
    chmod +x "$MILL_DIR/mill"
    echo "[02] mill $VER -> $JAR (自包含 jar + wrapper)"
else
    # 新版: 官方启动器脚本（按 .mill-version 下载对应版本到 ~/.cache/mill）
    echo "[02] mill $VER 无自包含 jar, 使用官方启动器（首次运行会下载 $VER 到 ~/.cache/mill）"
    curl -fsSL --retry 3 --retry-all-errors -o "$MILL_DIR/mill" \
        "${BASE}/mill-dist-${VER}-mill.sh"
    chmod +x "$MILL_DIR/mill"
    echo "[02] mill $VER -> $MILL_DIR/mill (官方启动器)"
fi

# 冒烟测试（flows/01 不 source env.sh, 这里显式优先用 tools/jdk 的 java）
# 注意: 在临时目录里跑, 避免 mill 在当前目录留下 out/ 抓痕
_smoke_bin="$(ls -d "$ROOT"/tools/jdk/*/bin 2>/dev/null | head -1)"
[ -n "$_smoke_bin" ] && PATH="$_smoke_bin:$PATH"
_smoke_dir="$(mktemp -d)"
( cd "$_smoke_dir" && "$MILL_DIR/mill" --version >/dev/null 2>&1 ) || {
    rm -rf "$_smoke_dir"
    echo "[02] ERROR: mill launcher 冒烟测试失败（检查 java: tools/jdk 或系统 JDK17+）"; exit 1; }
echo "[02] OK: mill $( cd "$_smoke_dir" && "$MILL_DIR/mill" --version 2>/dev/null | head -1 )"
rm -rf "$_smoke_dir"
