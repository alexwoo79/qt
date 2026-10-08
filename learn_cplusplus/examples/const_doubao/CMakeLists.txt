cmake_minimum_required(VERSION 3.16)

project(const_doubao LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(const_doubao main.cpp Student.cpp)
