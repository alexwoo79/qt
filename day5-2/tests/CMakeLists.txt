# tests：脱离界面测试业务逻辑。
# core 是零 UI 依赖的静态库，所以这里不需要窗口、不需要 GTK，直接跑。
add_executable(person_test person_test.cpp)
target_link_libraries(person_test PRIVATE day5_core)
target_compile_options(person_test PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME person_test COMMAND person_test)
