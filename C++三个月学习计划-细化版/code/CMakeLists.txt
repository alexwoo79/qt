cmake_minimum_required(VERSION 3.16)
project(cpp_deep_dive LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 每个 pN_MM_*.cpp 编译成一个独立可执行文件
file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS "p*.cpp")
foreach(src ${EXAMPLE_SOURCES})
  get_filename_component(name ${src} NAME_WE)
  add_executable(${name} ${src})
  target_compile_options(${name} PRIVATE -Wall -Wextra -g -O0)
endforeach()

# Keep the standalone clang-tidy example in the compilation database.
add_executable(test_clang_tidy ../test_clang_tidy.cpp)
target_compile_options(test_clang_tidy PRIVATE -Wall -Wextra -g -O0)

# 三个"故意犯错"的例子用 ASan 单独编译：cmake --build build --target p3_03_overflow_asan
foreach(bug p3_03_overflow p3_04_use_after_free p3_05_double_free)
  add_executable(${bug}_asan ${bug}.cpp)
  target_compile_options(${bug}_asan PRIVATE -fsanitize=address -g -O0)
  target_link_options(${bug}_asan PRIVATE -fsanitize=address)
endforeach()
