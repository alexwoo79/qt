# app：界面外壳（可执行目标 day5_2）。
#
# 它是唯一允许同时看到 core 和界面库的地方：
# 规则一在这里体现为 target_link_libraries —— core 不被允许反向依赖 app。

# ---- 依赖：GTK 3 + WebKitGTK（webview 在 Linux 上的后端）----
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED IMPORTED_TARGET gtk+-3.0)
pkg_check_modules(WEBKITGTK REQUIRED IMPORTED_TARGET webkit2gtk-4.1)

# ---- 把 web/index.html 内嵌成 C++ 头文件 ----
# 注意：index.html 里不要出现 CMake 的 “${...}” 写法，否则会在配置阶段被展开。
set(WEB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/web")
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_HEADER "${GENERATED_DIR}/index_html.h")

file(READ "${WEB_DIR}/index.html" INDEX_HTML)
file(MAKE_DIRECTORY "${GENERATED_DIR}")
file(WRITE "${GENERATED_HEADER}"
    "// 该文件由 CMake 从 app/web/index.html 生成，请勿手动修改。\n"
    "static const char *kIndexHtml = R\"WEBVIEW_HTML(\n${INDEX_HTML})WEBVIEW_HTML\";\n"
)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${WEB_DIR}/index.html")

# ---- 可执行程序 ----
# 职责切分：main = 组装，controller = 翻译，protocol = 编解码，presenter = 文案。
add_executable(day5_2
    main.cpp
    controller.cpp
    protocol.cpp
    presenter.cpp
)

target_include_directories(day5_2 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"  # controller.h / protocol.h / presenter.h
    "${GENERATED_DIR}"             # 生成的 index_html.h
    "${DAY5_THIRD_PARTY_DIR}"      # webview/webview.h
)

target_link_libraries(day5_2 PRIVATE
    day5_core
    nlohmann_json::nlohmann_json
    PkgConfig::GTK
    PkgConfig::WEBKITGTK
    ${CMAKE_DL_LIBS}
)

target_compile_options(day5_2 PRIVATE
    -Wall -Wextra -Wpedantic
)
