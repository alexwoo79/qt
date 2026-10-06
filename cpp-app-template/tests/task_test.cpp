// 业务测试：不启动界面，直接调用 core 层的函数。
// 没有引入测试框架——assert 风格的检查就够小项目用了，少一个依赖少一件事。
#include "myapp/task.h"

#include <iostream>
#include <string>

namespace {

int g_failures = 0;

void check(bool condition, const std::string &what) {
    if (condition) {
        std::cout << "ok   " << what << '\n';
    } else {
        std::cout << "FAIL " << what << '\n';
        ++g_failures;
    }
}

void empty_title_is_rejected() {
    myapp::TaskService service;
    const auto result = service.add("   ");

    check(!result.ok(), "只有空白的标题被拒绝");
    check(result.error.key == "task.title.empty", "失败原因是 task.title.empty");
    check(result.error.code == myapp::ErrorCode::invalid_input, "失败分类是 invalid_input");
    check(service.tasks().empty(), "被拒绝的输入不会写进列表");
}

void title_is_trimmed_and_stored_as_data() {
    myapp::TaskService service;
    const auto result = service.add("  写测试  ");

    check(result.ok(), "合法输入通过");
    check(result.value.title == "写测试", "标题去掉了首尾空白");
    check(service.tasks().size() == 1, "列表里有一条记录");
    check(service.tasks()[0].title == "写测试", "存的是数据本身，不是格式化后的字符串");
}

void order_is_preserved() {
    myapp::TaskService service;
    service.add("第一");
    service.add("第二");

    check(service.tasks().size() == 2, "两条都写进去了");
    check(service.tasks()[0].title == "第一" && service.tasks()[1].title == "第二",
          "保持先来先后的顺序");
}

void clear_empties_the_list() {
    myapp::TaskService service;
    service.add("写测试");
    service.clear();

    check(service.tasks().empty(), "清空之后列表为空");
}

} // namespace

int main() {
    empty_title_is_rejected();
    title_is_trimmed_and_stored_as_data();
    order_is_preserved();
    clear_empties_the_list();

    if (g_failures == 0) {
        std::cout << "\n全部通过\n";
        return 0;
    }
    std::cout << "\n失败 " << g_failures << " 项\n";
    return 1;
}
