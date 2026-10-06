#include "presenter.h"

namespace day5_2 {

namespace {

constexpr const char *kIdleText = "等待输入...";
constexpr const char *kClearedText = "已清空";

} // namespace

std::string Presenter::idleStatus() const {
    return kIdleText;
}

std::string Presenter::clearedStatus() const {
    return kClearedText;
}

std::string Presenter::formatPerson(const Person &person) const {
    return person.name + " (" + std::to_string(person.age) + " 岁)";
}

std::vector<std::string> Presenter::formatPeople(const std::vector<Person> &people) const {
    std::vector<std::string> lines;
    lines.reserve(people.size());
    for (const Person &person : people)
        lines.push_back(formatPerson(person));
    return lines;
}

std::string Presenter::submittedStatus(const Person &person) const {
    return "✅ 已添加: " + formatPerson(person);
}

std::string Presenter::messageFor(const Error &error) const {
    return lookup(error.code, error.key);
}

std::string Presenter::failedStatus(const Error &error) const {
    return "❌ " + messageFor(error);
}

std::string Presenter::lookup(ErrorCode code, const std::string &key) {
    // 先按 key 精确匹配：每个业务域可以有自己更具体的说法。
    if (key == "person.name.empty")
        return "姓名不能为空";
    if (key == "person.age.out_of_range")
        return "年龄必须在 1-150 之间";
    if (key == "protocol.bad_args")
        return "前端传来的参数无法解析";

    // 再按错误码兜底，保证任何 key 都有话说。
    switch (code) {
    case ErrorCode::invalid_input:
        return "输入不合法";
    case ErrorCode::not_found:
        return "没有找到对应的记录";
    case ErrorCode::conflict:
        return "操作冲突";
    case ErrorCode::internal:
        return "内部错误";
    case ErrorCode::none:
        break;
    }
    return "未知错误";
}

} // namespace day5_2
