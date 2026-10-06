#!/usr/bin/env bash
#
# Build the Windows installer (NSIS) for calculator_qml_win64/.
#
# 说明：NSIS 官方只提供 Windows 版，但 Linux 上可以直接用发行版的 makensis
# （原生 ELF 程序）。本脚本优先使用系统里的 makensis；没有的话就从 Ubuntu 源
# 下载 nsis + nsis-common 两个包解压到缓存目录里用，不需要 root。
#
# 用法：
#   ./build-installer.sh
#   NSIS_CACHE=/path/to/nsis ./build-installer.sh
#   UBUNTU_MIRROR=https://mirrors.ustc.edu.cn/ubuntu ./build-installer.sh
#
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
APP_DIR="${APP_DIR:-$HERE/calculator_qml_win64}"
NSI="${NSI:-$HERE/installer/calculator_qml.nsi}"
APP_VERSION="${APP_VERSION:-1.0.0}"
OUTFILE="${OUTFILE:-$HERE/calculator_qml-setup-$APP_VERSION.exe}"

NSIS_CACHE="${NSIS_CACHE:-$HOME/.cache/nsis}"
UBUNTU_MIRROR="${UBUNTU_MIRROR:-https://mirrors.tuna.tsinghua.edu.cn/ubuntu}"
NSIS_DEB_VER="${NSIS_DEB_VER:-3.10-2ubuntu2}"
NSIS_DEB_DIR="$UBUNTU_MIRROR/pool/universe/n/nsis"

[ -d "$APP_DIR" ] || { echo "找不到程序目录：$APP_DIR" >&2; echo "先运行 ./build-windows.sh" >&2; exit 1; }

makensis_cmd=""
nsisdir=""

resolve_nsis() {
    if command -v makensis >/dev/null 2>&1; then
        makensis_cmd="$(command -v makensis)"
        if [ -d /usr/share/nsis ]; then
            nsisdir="/usr/share/nsis"
        fi
        return
    fi

    local root="$NSIS_CACHE/root"
    if [ ! -x "$root/usr/bin/makensis" ] || [ ! -d "$root/usr/share/nsis/Plugins" ]; then
        echo "==> 下载 NSIS（makensis）到 $NSIS_CACHE"
        mkdir -p "$NSIS_CACHE/dl" "$root"
        cd "$NSIS_CACHE/dl"
        for deb in "nsis_${NSIS_DEB_VER}_amd64.deb" "nsis-common_${NSIS_DEB_VER}_all.deb"; do
            [ -f "$deb" ] || curl -fL --retry 3 -O "$NSIS_DEB_DIR/$deb"
            rm -rf "$NSIS_CACHE/.tmp" && mkdir -p "$NSIS_CACHE/.tmp"
            bsdtar -xf "$deb" -C "$NSIS_CACHE/.tmp"
            bsdtar -xf "$NSIS_CACHE/.tmp"/data.tar.* -C "$root"
        done
        rm -rf "$NSIS_CACHE/.tmp"
    fi

    makensis_cmd="$root/usr/bin/makensis"
    nsisdir="$root/usr/share/nsis"
}

resolve_nsis
export NSISDIR="$nsisdir"
echo "==> makensis: $makensis_cmd"
echo "==> NSISDIR : $NSISDIR"

echo "==> 编译安装程序（LZMA solid 压缩，需要一点时间）"
cd "$HERE"
"$makensis_cmd" -V2 -DNSISDIR="$NSISDIR" -DSRCDIR="$APP_DIR" -DOUTFILE="$OUTFILE" "$NSI"

[ -f "$OUTFILE" ] || { echo "生成失败：$OUTFILE" >&2; exit 1; }

echo
echo "安装程序：$OUTFILE"
ls -lh "$OUTFILE" | awk '{print "大小："$5}'
