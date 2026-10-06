cmake_minimum_required(VERSION 3.16)
project(myapp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# 第三方单头文件（webview、nlohmann/json）所在目录，谁需要谁引用。
set(MYAPP_THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")

# nlohmann/json：包成 INTERFACE 目标，并对齐官方包提供的目标名 nlohmann_json::nlohmann_json，
# 这样以后想换成 find_package(nlohmann_json REQUIRED)，各外壳的链接处一行都不用改。
add_library(nlohmann_json_vendored INTERFACE)
target_include_directories(nlohmann_json_vendored INTERFACE "${MYAPP_THIRD_PARTY_DIR}")
add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json_vendored)

# 分层：core = 纯 C++ 业务逻辑；app = 界面外壳；tests = 脱离界面的业务测试。
# 依赖方向只有一条：app -> core。core 的目标里没有任何界面库，
# 所以“业务层不许碰界面”这条规则由构建系统强制，不靠自觉。
enable_testing()
add_subdirectory(core)
add_subdirectory(app)
add_subdirectory(tests)
