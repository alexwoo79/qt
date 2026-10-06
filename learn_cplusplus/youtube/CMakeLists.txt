cmake_minimum_required(VERSION 3.16)
project(youtube_webview LANGUAGES CXX)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(WEBKITGTK REQUIRED IMPORTED_TARGET webkitgtk-6.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_HEADER "${GENERATED_DIR}/index_html.h")

file(READ "${SOURCE_DIR}/index.html" INDEX_HTML)
file(MAKE_DIRECTORY "${GENERATED_DIR}")
file(WRITE "${GENERATED_HEADER}"
  "// Generated from src/index.html; do not edit.\n"
  "static const char *kIndexHtml = R\"WEBVIEW_HTML(\n${INDEX_HTML})WEBVIEW_HTML\";\n"
)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${SOURCE_DIR}/index.html"
)

add_executable(myapp "${SOURCE_DIR}/cppwebview.cpp")
target_include_directories(myapp PRIVATE "${GENERATED_DIR}")
target_compile_options(myapp PRIVATE -Wall)
target_link_libraries(myapp PRIVATE
  PkgConfig::GTK4
  PkgConfig::WEBKITGTK
  ${CMAKE_DL_LIBS}
)
