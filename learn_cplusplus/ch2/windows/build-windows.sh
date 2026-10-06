#!/usr/bin/env bash
#
# Cross-compile the Qt Quick calculator (calculator_qml.cpp + Main.qml) from
# Linux to a portable Windows x86_64 application.
#
# The build uses:
#   * MinGW-w64 GCC as the cross compiler (x86_64-w64-mingw32-g++)
#   * the official Qt for Windows (MinGW) libraries, downloaded on first run
#   * the native Linux Qt of the same version as host tools (moc, rcc, ...)
#
# Requirements on Arch/Omarchy:
#   sudo pacman -S mingw-w64-gcc cmake ninja curl libarchive qt6-base qt6-declarative
#
# Usage:
#   ./build-windows.sh                       # build, deploy, then verify
#   QT_WIN_SDK=/path/to/qt-win-sdk ./build-windows.sh
#   QT_MIRROR=https://download.qt.io/qt ./build-windows.sh
#
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
SRC="$(cd "$HERE/.." && pwd)"
MINGW_TRIPLE=x86_64-w64-mingw32
OBJDUMP="${MINGW_TRIPLE}-objdump"
MINGW_RUNTIME="/usr/${MINGW_TRIPLE}/bin"

# --- Qt for Windows (MinGW) package -----------------------------------------
QT_VERSION=6.11.2
QT_TS="6.11.2-0-202608131017"                       # snapshot stamp of this release
QT_MIRROR="${QT_MIRROR:-https://mirrors.tuna.tsinghua.edu.cn/qt}"
QT_REPO="$QT_MIRROR/online/qtsdkrepository/windows_x86/desktop/qt6_6112"
QT_SDK_DIR="${QT_WIN_SDK:-$HOME/.cache/qt-win-sdk/$QT_VERSION}"

QT_ARCHIVES=(
    "qt6_6112_mingw/qt.qt6.6112.win64_mingw/${QT_TS}qtbase-Windows-Windows_11_24H2-Mingw-Windows-Windows_11_24H2-X86_64.7z"
    "qt6_6112_mingw/qt.qt6.6112.win64_mingw/${QT_TS}qtdeclarative-Windows-Windows_11_24H2-Mingw-Windows-Windows_11_24H2-X86_64.7z"
    "qt6_6112_mingw/qt.qt6.6112.win64_mingw/${QT_TS}MinGW-w64-x86_64-13.1.0-release-posix-seh-msvcrt-rt_v11-rev1-runtime.7z"
    "qt6_6112_mingw/qt.qt6.6112.win64_mingw/${QT_TS}opengl32sw-64-mesa_11_2_2-signed_sha256.7z"
    "qt6_6112_mingw/qt.qt6.6112.win64_mingw/${QT_TS}d3dcompiler_47-x64.7z"
    "qt6_6112_mingw/qt.qt6.6112.addons.qtshadertools.win64_mingw/${QT_TS}qtshadertools-Windows-Windows_11_24H2-Mingw-Windows-Windows_11_24H2-X86_64.7z"
)

BUILD_DIR="${BUILD_DIR:-$HERE/.build-win64}"
OUT="$HERE/calculator_qml_win64"

for tool in "$MINGW_TRIPLE-g++" cmake ninja curl bsdtar "$OBJDUMP"; do
    command -v "$tool" >/dev/null || { echo "missing required tool: $tool" >&2; exit 1; }
done

ensure_qt_sdk() {
    if [ -f "$QT_SDK_DIR/lib/cmake/Qt6/Qt6Config.cmake" ]; then
        echo "==> using Qt for Windows in $QT_SDK_DIR"
        return
    fi

    echo "==> downloading Qt $QT_VERSION for Windows (MinGW) into $QT_SDK_DIR"
    mkdir -p "$QT_SDK_DIR"
    local download_dir="$QT_SDK_DIR/.downloads"
    mkdir -p "$download_dir"

    for archive in "${QT_ARCHIVES[@]}"; do
        local file="$download_dir/$(basename "$archive")"
        if [ ! -f "$file" ]; then
            echo "    $(basename "$archive")"
            curl -fL --retry 3 --retry-delay 2 -C - -o "$file" "$QT_REPO/$archive"
        fi
        bsdtar -xf "$file" -C "$QT_SDK_DIR"
    done

    echo "==> Qt for Windows ready"
}

configure_and_build() {
    echo "==> configuring"
    cmake -S "$SRC" -B "$BUILD_DIR" -G Ninja \
        -DCMAKE_TOOLCHAIN_FILE="$HERE/toolchain-mingw64.cmake" \
        -DQT_WIN_ROOT="$QT_SDK_DIR" \
        -DCMAKE_PREFIX_PATH="$QT_SDK_DIR" \
        -DQT_HOST_PATH=/usr \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_EXE_LINKER_FLAGS="-Wl,--subsystem,windows -Wl,-e,mainCRTStartup"

    echo "==> compiling"
    cmake --build "$BUILD_DIR"
}

deploy() {
    echo "==> assembling portable folder in $OUT"
    rm -rf "$OUT"
    mkdir -p "$OUT/plugins/platforms"

    cp "$BUILD_DIR/calculator_qml.exe" "$OUT/"

    # QML modules are resolved at run time. Instead of shipping the whole qml/
    # tree, ship exactly the modules qmlimportscanner found while compiling.
    local imports_file="$BUILD_DIR/.qt/qml_imports/calculator_qml_build.cmake"
    [ -f "$imports_file" ] || { echo "missing $imports_file" >&2; exit 1; }

    # qmlimportscanner lists every Qt Quick Controls style (Fusion/Material/
    # Universal/Imagine/...), because QtQuick.Controls can switch styles at run
    # time. This app imports one style explicitly, so keep only that one.
    local used_styles
    used_styles="$(grep -rhoE '^[[:space:]]*import[[:space:]]+QtQuick\.Controls\.[A-Za-z]+' "$SRC"/*.qml 2>/dev/null \
        | awk '{print $2}' | sed 's/.*\.//' | sort -u)"
    [ -n "$used_styles" ] || used_styles="Basic"
    echo "    control styles kept: $(echo "$used_styles" | tr '\n' ' ')"

    keep_qml_module() {
        local rel="$1" style
        case "$rel" in
            QtQuick/Controls/[A-Z]*)
                style="${rel#QtQuick/Controls/}"
                printf '%s\n' "$used_styles" | grep -qx "${style%%/*}"
                ;;
            QtQuick/NativeStyle|QtQuick/NativeStyle/*)
                printf '%s\n' "$used_styles" | grep -qx "NativeStyle"
                ;;
            *) return 0 ;;
        esac
    }

    local rel
    while IFS= read -r rel; do
        [ -n "$rel" ] || continue
        [ -d "$QT_SDK_DIR/qml/$rel" ] || continue
        keep_qml_module "$rel" || continue
        mkdir -p "$OUT/qml/$rel"
        find "$QT_SDK_DIR/qml/$rel" -maxdepth 1 -type f -exec cp {} "$OUT/qml/$rel/" \;
    done < <(grep -oE 'RELATIVEPATH;[^;"\\]+' "$imports_file" | sed 's/RELATIVEPATH;//' | sort -u)

    # Only the Windows platform plugin is required for this app.
    cp "$QT_SDK_DIR"/plugins/platforms/*.dll "$OUT/plugins/platforms/"
    rm -f "$OUT"/plugins/platforms/qdirect2d.dll \
          "$OUT"/plugins/platforms/qminimal.dll \
          "$OUT"/plugins/platforms/qoffscreen.dll
    # No imageformats plugins: the UI uses no bitmaps at all.

    # Transitive closure over the Qt DLLs that the exe and every plugin needs.
    local -a queue=("$OUT/calculator_qml.exe")
    local f
    while IFS= read -r -d '' f; do queue+=("$f"); done < <(find "$OUT" -name '*.dll' -print0)

    while [ ${#queue[@]} -gt 0 ]; do
        local current="${queue[0]}"
        queue=("${queue[@]:1}")
        local dep
        while IFS= read -r dep; do
            [ -n "$dep" ] || continue
            if [ -f "$QT_SDK_DIR/bin/$dep" ] && [ ! -f "$OUT/$dep" ]; then
                cp "$QT_SDK_DIR/bin/$dep" "$OUT/"
                queue+=("$OUT/$dep")
            fi
        done < <("$OBJDUMP" -p "$current" | awk '/DLL Name/{print $3}')
    done

    cp "$MINGW_RUNTIME/libgcc_s_seh-1.dll" \
       "$MINGW_RUNTIME/libstdc++-6.dll" \
       "$MINGW_RUNTIME/libwinpthread-1.dll" \
       "$OUT/"

    # Optional software rasterizer for machines with no usable GPU driver.
    # Windows 10/11 already ships d3dcompiler_47.dll, so neither is needed by
    # default; set WITH_SOFTWARE_RENDERER=1 to include them.
    if [ "${WITH_SOFTWARE_RENDERER:-0}" = "1" ]; then
        cp "$QT_SDK_DIR/opengl32sw.dll" "$QT_SDK_DIR/d3dcompiler_47.dll" "$OUT/"
    fi
}

verify() {
    echo "==> verifying dependencies"
    local system_re='^(api-ms-win-.*|ext-ms-win-.*|kernel32|user32|gdi32|advapi32|shell32|ole32|oleaut32|uuid|comdlg32|winspool|ws2_32|mpr|userenv|ntdll|msvcrt|ucrtbase|d3d9|d3d11|d3d12|dcomp|dxgi|dxguid|dwmapi|d2d1|dwrite|imm32|setupapi|shcore|shlwapi|winmm|wtsapi32|version|netapi32|authz|bcrypt|crypt32|secur32|uxtheme|opengl32|glu32|gdiplus|imagehlp|rpcrt4|dnsapi|iphlpapi|winhttp|wininet|normaliz|combase)$'
    local missing=0 f dep dep_lc

    while IFS= read -r -d '' f; do
        while IFS= read -r dep; do
            [ -n "$dep" ] || continue
            dep_lc="$(printf '%s' "$dep" | tr 'A-Z' 'a-z')"
            dep_lc="${dep_lc%.dll}"
            printf '%s' "$dep_lc" | grep -qE "$system_re" && continue
            if [ ! -f "$OUT/$dep" ]; then
                echo "    MISSING  $dep (required by ${f#"$OUT"/})"
                missing=1
            fi
        done < <("$OBJDUMP" -p "$f" 2>/dev/null | awk '/DLL Name/{print $3}')
    done < <(find "$OUT" \( -name '*.dll' -o -name '*.exe' \) -print0)

    # every QML module in the folder must bring its plugin library
    local qmldir plugin dir
    while IFS= read -r qmldir; do
        dir="$(dirname "$qmldir")"
        for plugin in $(grep -oE '^plugin[[:space:]]+[^[:space:]]+' "$qmldir" | awk '{print $2}' | tr -d '\r'); do
            [ -f "$dir/$plugin.dll" ] || { echo "    MISSING  $dir/$plugin.dll"; missing=1; }
        done
    done < <(find "$OUT/qml" -name qmldir)

    if [ "$missing" -ne 0 ]; then
        echo "==> FAILED: unresolved dependencies" >&2
        exit 1
    fi
    echo "==> OK: all dependencies resolve inside $OUT"
}

ensure_qt_sdk
configure_and_build
deploy
verify

echo
echo "Windows program: $OUT/calculator_qml.exe"
du -sh "$OUT"
