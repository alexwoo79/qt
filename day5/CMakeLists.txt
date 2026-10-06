cmake_minimum_required(VERSION 3.16)
project(day5 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)

qt_add_executable(day5
    main.cpp
    backend.cpp
    backend.h
)

qt_add_qml_module(day5
    URI day5
    VERSION 1.0
    QML_FILES Main.qml
)

target_link_libraries(day5 PRIVATE Qt6::Quick)
set_target_properties(day5 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)