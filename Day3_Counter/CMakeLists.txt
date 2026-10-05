cmake_minimum_required(VERSION 3.19)
project(Day3_Counter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 6.5 REQUIRED COMPONENTS Core LinguistTools)


qt_standard_project_setup()

qt_add_executable(Day3_Counter
    main.cpp
    counter.cpp
    counter.h
)

qt_add_translations(
    TARGETS Day3_Counter
    TS_FILES Day3_Counter_zh_CN.ts
)

target_link_libraries(Day3_Counter
    PRIVATE
        Qt::Core
)

install(TARGETS Day3_Counter
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
