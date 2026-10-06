#!/usr/bin/env bash
#
# 用这个模板开一个新项目：把模板里的 myapp 全部换成你的项目名。
#
#   ./scripts/new-project.sh <目标目录> <项目名>
#   ./scripts/new-project.sh ~/work/pomodoro pomodoro
#
# 项目名会同时变成 C++ 命名空间和可执行文件名，所以只能用小写字母、
# 数字、下划线，且以字母开头（C++ 标识符的规则）。
set -euo pipefail

TEMPLATE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

usage() {
    echo "用法: $(basename "$0") <目标目录> <项目名>" >&2
    echo "例:   $(basename "$0") ~/work/pomodoro pomodoro" >&2
}

[ $# -eq 2 ] || { usage; exit 1; }

TARGET_DIR="$1"
NEW_NAME="$2"

if ! printf '%s' "$NEW_NAME" | grep -Eq '^[a-z][a-z0-9_]*$'; then
    echo "项目名只能用小写字母、数字、下划线，且以字母开头：$NEW_NAME" >&2
    exit 1
fi

if [ -e "$TARGET_DIR" ]; then
    echo "目标已存在，换个路径或先处理掉它：$TARGET_DIR" >&2
    exit 1
fi

mkdir -p "$TARGET_DIR"

# 整棵模板树复制过去，跳过构建产物和版本库。
# scripts/ 不复制：它是"生成器"，属于模板自己，生成出来的项目里不该带着它。
tar -C "$TEMPLATE_DIR" \
    --exclude=./build \
    --exclude=./build-* \
    --exclude=./.git \
    --exclude=./scripts \
    -cf - . | tar -C "$TARGET_DIR" -xf -

# 1) 改文件名 / 目录名（-depth 保证先改深层，再改上层）
find "$TARGET_DIR" -depth -name '*myapp*' -print0 |
    while IFS= read -r -d '' path; do
        renamed="$(dirname "$path")/$(basename "$path" | sed "s/myapp/$NEW_NAME/g")"
        mv "$path" "$renamed"
    done

# 2) 改文件内容：myapp / MYAPP / MyApp 三种写法一起处理
UPPER_NAME="$(printf '%s' "$NEW_NAME" | tr '[:lower:]' '[:upper:]')"
while IFS= read -r file; do
    sed -i "s/myapp/$NEW_NAME/g; s/MYAPP/$UPPER_NAME/g; s/MyApp/$NEW_NAME/g" "$file"
done < <(grep -rl --exclude-dir=.git 'myapp\|MYAPP\|MyApp' "$TARGET_DIR" || true)

echo "已创建项目：$TARGET_DIR"
echo
echo "下一步："
echo "  cd $TARGET_DIR"
echo "  cmake -S . -B build -G Ninja && cmake --build build"
echo "  cd build && ctest --output-on-failure"
echo
echo "然后就可以开始改 core/include/${NEW_NAME}/ 下的业务代码了。"
