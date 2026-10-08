#!/usr/bin/env bash
# 一键编译并运行所有例题，输出存到 outputs/<名字>.txt
# 用法：./run_all.sh            （普通编译）
#      ./run_all.sh asan        （额外用 ASan 跑三个内存错误例子）
set -uo pipefail
cd "$(dirname "$0")"
mkdir -p outputs

for src in p*.cpp; do
  name="${src%.cpp}"
  case "$name" in
    p3_03_overflow|p3_04_use_after_free|p3_05_double_free) continue ;;  # 这三个只在 ASan 下跑
  esac
  echo "== $name"
  if ! clang++ -std=c++17 -Wall -g -O0 "$src" -o "outputs/$name" 2> "outputs/$name.build.log"; then
    echo "   编译失败，见 outputs/$name.build.log"
    continue
  fi
  "./outputs/$name" > "outputs/$name.txt" 2>&1 || echo "   运行退出码非 0（看 outputs/$name.txt）"
done

for bug in p3_03_overflow p3_04_use_after_free p3_05_double_free; do
  echo "== $bug (ASan)"
  clang++ -std=c++17 -g -O0 -fsanitize=address "$bug.cpp" -o "outputs/$bug.asan" 2>/dev/null
  ASAN_OPTIONS=abort_on_error=0 "./outputs/$bug.asan" > "outputs/$bug.asan.txt" 2>&1 || true
done
echo "完成，结果在 outputs/"
