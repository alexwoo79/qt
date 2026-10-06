# 声明本项目要求的最低 CMake 版本；3.16 提供这里使用的项目与目标配置能力。
cmake_minimum_required(VERSION 3.16)
# 定义项目名称为 youtube_webview，并声明项目只使用 C++ 语言进行构建。
project(youtube_webview LANGUAGES CXX)

# 查找 pkg-config 程序；REQUIRED 表示未安装时立即停止配置并报告错误。
find_package(PkgConfig REQUIRED)
# 通过 pkg-config 查找 GTK 4 开发库，并创建可供目标链接的导入目标 PkgConfig::GTK4。
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
# 查找 WebKitGTK 6.0 开发库，并创建可供目标链接的导入目标 PkgConfig::WEBKITGTK。
pkg_check_modules(WEBKITGTK REQUIRED IMPORTED_TARGET webkitgtk-6.0)

# 将本项目源文件使用的 C++ 标准设为 C++11。
set(CMAKE_CXX_STANDARD 11)
# 要求编译器必须支持所指定的 C++ 标准，不允许静默降级。
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 禁止启用编译器特有的 C++ 扩展，以使用标准 C++11 语法。
set(CMAKE_CXX_EXTENSIONS OFF)

# 保存源码目录路径；CMAKE_CURRENT_SOURCE_DIR 指当前 CMakeLists.txt 所在目录。
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
# 保存生成文件目录路径；CMAKE_CURRENT_BINARY_DIR 指当前构建目录，而非源码目录。
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
# 保存将要生成的 C++ 头文件完整路径，该文件内嵌网页 HTML 内容。
set(GENERATED_HEADER "${GENERATED_DIR}/index_html.h")

# 读取 src/index.html 的全部内容，并将内容保存到 INDEX_HTML 变量供后续生成头文件使用。
file(READ "${SOURCE_DIR}/index.html" INDEX_HTML)
# 确保生成目录存在；目录已存在时此操作不会造成问题。
file(MAKE_DIRECTORY "${GENERATED_DIR}")
# 将后续给出的字符串写入生成的头文件，以便 C++ 源文件编译时包含网页内容。
file(WRITE "${GENERATED_HEADER}"
  # 在生成的头文件中写入提示，说明文件由 src/index.html 生成，不应手动编辑。
  "// Generated from src/index.html; do not edit.\n"
  # 生成 C++ 原始字符串常量的声明，并以自定义分隔符开始承载 HTML 的内容。
  "static const char *kIndexHtml = R\"WEBVIEW_HTML(\n${INDEX_HTML})WEBVIEW_HTML\";\n"
)
# 将网页源文件登记为当前目录配置过程的依赖项，使它变化时 CMake 重新执行配置。
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  # 指定需要监视的 HTML 文件路径；文件内容变化会触发重新配置并重新生成头文件。
  "${SOURCE_DIR}/index.html"
)

# 创建名为 myapp 的可执行程序，并以 cppwebview.cpp 作为它的 C++ 源文件。
add_executable(myapp "${SOURCE_DIR}/cppwebview.cpp")
# 将生成目录设为 myapp 的私有头文件搜索路径，使源码可以包含 index_html.h。
target_include_directories(myapp PRIVATE "${GENERATED_DIR}")
# 为 myapp 启用 -Wall 编译选项，要求编译器报告常见警告。
target_compile_options(myapp PRIVATE -Wall)
# 将 GTK、WebKitGTK 及平台动态加载库链接到 myapp；PRIVATE 表示依赖不向其他目标传递。
target_link_libraries(myapp PRIVATE
  # 链接前面由 pkg-config 创建的 GTK 4 导入目标，并自动应用其头文件及链接参数。
  PkgConfig::GTK4
  # 链接前面由 pkg-config 创建的 WebKitGTK 导入目标，并自动应用其依赖参数。
  PkgConfig::WEBKITGTK
  # 链接平台对应的动态加载库；在需要时提供 dlopen 等动态库加载相关函数。
  ${CMAKE_DL_LIBS}
)
