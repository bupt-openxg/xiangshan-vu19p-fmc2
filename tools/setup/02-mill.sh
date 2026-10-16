#!/usr/bin/env bash
# 02 mill (版本 = XiangShan/.mill-version, 当前 0.12.3) -> tools/mill/
#   注意: GitHub 的 -assembly jar 无 Main-Class 不可直接运行; 使用 Maven Central 的
#   mill-dist 自执行 jar, 经 tools/mill/mill wrapper 调用 MillClientMain
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VER="$(cat "$ROOT/third_party/XiangShan/.mill-version" 2>/dev/null || echo 0.12.3)"
JAR="$ROOT/tools/mill/mill-dist-${VER}.jar"
[ -f "$JAR" ] || curl -L --fail -o "$JAR" "https://repo1.maven.org/maven2/com/lihaoyi/mill-dist/${VER}/mill-dist-${VER}.jar"
chmod +x "$ROOT/tools/mill/mill"
echo "[02] mill $VER -> $JAR (launcher: tools/mill/mill)"
