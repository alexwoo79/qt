cmake_minimum_required(VERSION 3.21)

project(LearnCppCalculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 没有指定构建类型时默认用 Release（体积/启动优化都以 Release 为前提）
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick QuickControls2)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(calculator_qml
    calculator_qml.cpp
)

qt_add_qml_module(calculator_qml
    URI LearnCpp.Calculator
    VERSION 1.0
    QML_FILES
        Main.qml
)

# QML 源文件不再嵌入二进制，只保留编译后的产物（Qt 6.6+）。
# 进一步缩小 exe，也让“源码泄漏”这件事不存在。
set_target_properties(calculator_qml PROPERTIES
    QT_DISCARD_FILE_CONTENTS TRUE
)

# 以体积优先优化，并让链接器丢弃没被引用到的代码段
if(MSVC)
    target_compile_options(calculator_qml PRIVATE /O1 /Gy /Gw)
    target_link_options(calculator_qml PRIVATE /OPT:REF /OPT:ICF)
else()
    target_compile_options(calculator_qml PRIVATE -Os -ffunction-sections -fdata-sections)
    target_link_options(calculator_qml PRIVATE -Wl,--gc-sections)
endif()

target_link_libraries(calculator_qml
    PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
)
