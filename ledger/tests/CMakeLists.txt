# tests：脱离界面测试业务逻辑。
# core 是零 UI 依赖的静态库，所以这里不需要窗口、不需要 GTK，直接跑。
#
# 新增业务模块时，加一个 *_test.cpp + 一个 add_executable/add_test 即可。
add_executable(ledger_test ledger_test.cpp)
target_link_libraries(ledger_test PRIVATE ledger_core)
target_compile_options(ledger_test PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME ledger_test COMMAND ledger_test)
