#!/usr/bin/env bash
# 02 mill (版本 = XiangShan/.mill-version, 当前 0.12.3) -> tools/mill/
#   注意: GitHub 的 -assembly jar 无 Main-Class 不可直接运行; Maven Central 的 mill-dist jar
#   同样无 Main-Class, 但含入口类 mill.runner.client.MillClientMain —— 由本脚本自生成的
#   tools/mill/mill 包装脚本用 `java -cp <jar> mill.runner.client.MillClientMain` 调用。
#   （tools/mill/ 整个目录被 gitignore, 全新克隆时必须由本脚本创建。）
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VER="$(cat "$ROOT/third_party/XiangShan/.mill-version" 2>/dev/null || echo 0.12.3)"
MILL_DIR="$ROOT/tools/mill"
mkdir -p "$MILL_DIR"
JAR="$MILL_DIR/mill-dist-${VER}.jar"
[ -f "$JAR" ] || curl -L --fail -o "$JAR" "https://repo1.maven.org/maven2/com/lihaoyi/mill-dist/${VER}/mill-dist-${VER}.jar"

# 包装脚本（已存在则不覆盖）
if [ ! -f "$MILL_DIR/mill" ]; then
cat > "$MILL_DIR/mill" <<'WRAPPER'
#!/usr/bin/env bash
# mill launcher（由 tools/setup/02-mill.sh 生成）: 运行 mill-dist 自执行 jar
# 用法: mill [mill 参数...]   —— 需要 JAVA_HOME 已指向 JDK（本仓库: source tools/env.sh）
set -euo pipefail
_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
_jar="$(ls "$_dir"/mill-dist-*.jar 2>/dev/null | head -1)"
[ -n "$_jar" ] || { echo "mill-dist jar 缺失（先运行 tools/setup/02-mill.sh）" >&2; exit 1; }
exec java -cp "$_jar" mill.runner.client.MillClientMain "$@"
WRAPPER
fi
chmod +x "$MILL_DIR/mill"

# 冒烟测试（flows/01 不 source env.sh, 这里显式优先用 tools/jdk 的 java）
_smoke_bin="$(ls -d "$ROOT"/tools/jdk/*/bin 2>/dev/null | head -1)"
[ -n "$_smoke_bin" ] && PATH="$_smoke_bin:$PATH"
"$MILL_DIR/mill" --version >/dev/null 2>&1 || {
    echo "[02] ERROR: mill launcher 冒烟测试失败（检查 java: tools/jdk 或系统 JDK17+）"; exit 1; }
echo "[02] OK: mill $("$MILL_DIR/mill" --version 2>/dev/null | head -1) -> $JAR (launcher: tools/mill/mill)"
