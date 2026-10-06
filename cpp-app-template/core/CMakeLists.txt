# myapp_core：纯 C++ 业务逻辑（core/ 层）。
#
# 规则一：依赖只朝一个方向。这个目标的链接列表里没有 GTK、没有 webview，
# 所以谁在 core 里 include 界面头文件，编译会直接失败——靠构建系统守规矩。
# 规则二：一个目录一种角色。core/ 只放业务，界面代码一律放 app/。
# 规则三：公开头文件在 include/<项目名>/ 下，外部按 "myapp/xxx.h" 引用。
#
# 新增业务模块时，把新的 .cpp 加进下面的列表即可（一个模块一个文件，
# 别急着拆子目录——等真的出现第二个业务域再分）。
add_library(myapp_core STATIC
    src/task.cpp
)

target_include_directories(myapp_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# 规则四：错误处理策略和告警级别第一天就定死，不靠事后 review。
target_compile_options(myapp_core PRIVATE
    -Wall -Wextra -Wpedantic
)
