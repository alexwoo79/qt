#pragma once

#include "day5_2/person.h"
#include "day5_2/result.h"

#include <string>
#include <vector>

namespace day5_2 {

// 展示层：整个项目里唯一生成面向用户文本的地方。
//
// core 只给"事实"（成功的数据，或错误码 + key），这里负责把事实变成人话。
// 换语言、换图标、换成英文界面，都只改这一个类。
class Presenter {
public:
    std::string idleStatus() const;
    std::string clearedStatus() const;
    std::string submittedStatus(const Person &person) const;
    std::string failedStatus(const Error &error) const;

    // 把一条记录格式化成列表里显示的样子。
    std::string formatPerson(const Person &person) const;
    std::vector<std::string> formatPeople(const std::vector<Person> &people) const;

    // 错误码 / 错误 key -> 中文说明。
    std::string messageFor(const Error &error) const;

private:
    static std::string lookup(ErrorCode code, const std::string &key);
};

} // namespace day5_2
