#!/usr/bin/env bash
# 01 GraalVM CE JDK 21 (香山官方 xs-env 指定版本线) -> tools/jdk/
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
JDK_DIR="$ROOT/tools/jdk"
TARBALL="$JDK_DIR/graalvm-community-jdk-21.0.2.tar.gz"
URL="https://github.com/graalvm/graalvm-ce-builds/releases/download/jdk-21.0.2/graalvm-community-jdk-21.0.2_linux-x64_bin.tar.gz"
mkdir -p "$JDK_DIR"
[ -f "$TARBALL" ] || curl -L --fail -o "$TARBALL" "$URL"
ls -d "$JDK_DIR"/graalvm-community-openjdk-21* >/dev/null 2>&1 || tar -xzf "$TARBALL" -C "$JDK_DIR"
"$(ls -d "$JDK_DIR"/graalvm-community-openjdk-21*/ | head -1)bin/java" -version
echo "[01] OK"
