// 业务测试：不启动界面，直接调用 core 层的函数。
// 没有引入测试框架——assert 风格的检查就够小项目用了，少一个依赖少一件事。
#include "day5_2/person.h"

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

void empty_name_is_rejected() {
    day5_2::PersonService service;
    const auto result = service.submit("   ", 20);

    check(!result.ok(), "只有空白的姓名被拒绝");
    check(result.error.key == "person.name.empty", "失败原因是 person.name.empty");
    check(result.error.code == day5_2::ErrorCode::invalid_input, "失败分类是 invalid_input");
    check(service.people().empty(), "被拒绝的输入不会写进列表");
}

void age_out_of_range_is_rejected() {
    day5_2::PersonService service;

    check(!service.submit("张三", 0).ok(), "年龄 0 被拒绝");
    check(!service.submit("张三", 151).ok(), "年龄 151 被拒绝");
    check(service.people().empty(), "越界的输入一条都没写进去");
}

void age_boundaries_are_accepted() {
    day5_2::PersonService service;

    check(service.submit("最小", 1).ok(), "年龄 1 通过");
    check(service.submit("最大", 150).ok(), "年龄 150 通过");
    check(service.people().size() == 2, "两条边界记录都写进去了");
}

void name_is_trimmed_and_stored_as_data() {
    day5_2::PersonService service;
    const auto result = service.submit("  张三  ", 20);

    check(result.ok(), "合法输入通过");
    check(result.value.name == "张三", "姓名去掉了首尾空白");
    check(result.value.age == 20, "年龄原样保留");
    check(service.people().size() == 1, "列表里有一条记录");
    check(service.people()[0].name == "张三", "存的是数据本身，不是格式化后的字符串");
}

void clear_empties_the_list() {
    day5_2::PersonService service;
    service.submit("张三", 20);
    service.clear();

    check(service.people().empty(), "清空之后列表为空");
}

} // namespace

int main() {
    empty_name_is_rejected();
    age_out_of_range_is_rejected();
    age_boundaries_are_accepted();
    name_is_trimmed_and_stored_as_data();
    clear_empties_the_list();

    if (g_failures == 0) {
        std::cout << "\n全部通过\n";
        return 0;
    }
    std::cout << "\n失败 " << g_failures << " 项\n";
    return 1;
}
