# CMake toolchain: cross-compile a Linux-hosted build for Windows x86_64 (MinGW-w64).
#
# Used by build-windows.sh:
#   cmake -S .. -B <build> -G Ninja \
#         -DCMAKE_TOOLCHAIN_FILE=toolchain-mingw64.cmake \
#         -DQT_WIN_ROOT=<Qt for Windows (MinGW) prefix> \
#         -DQT_HOST_PATH=/usr
#
# QT_HOST_PATH points at the native Linux Qt installation; the build tools it
# provides (moc, rcc, qmlcachegen, qmltyperegistrar, qmlimportscanner) run on the
# host, while the Qt libraries that get linked are the Windows ones in
# QT_WIN_ROOT.

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(MINGW_TRIPLE x86_64-w64-mingw32)

set(CMAKE_C_COMPILER   ${MINGW_TRIPLE}-gcc)
set(CMAKE_CXX_COMPILER ${MINGW_TRIPLE}-g++)
set(CMAKE_RC_COMPILER  ${MINGW_TRIPLE}-windres)
set(CMAKE_AR           ${MINGW_TRIPLE}-ar)
set(CMAKE_RANLIB       ${MINGW_TRIPLE}-ranlib)
set(CMAKE_STRIP        ${MINGW_TRIPLE}-strip)

set(QT_WIN_ROOT "" CACHE PATH "Qt for Windows (MinGW) installation prefix")

set(CMAKE_FIND_ROOT_PATH /usr/${MINGW_TRIPLE} ${QT_WIN_ROOT})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
