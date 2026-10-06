#pragma once

#include "day5_2/result.h"

#include <string>
#include <string_view>
#include <vector>

namespace day5_2 {

// 一条记录的业务数据。这里只存事实，不存任何展示用的文本。
struct Person {
    std::string name;
    int age = 0;
};

// 人员列表的业务逻辑：校验、保存、清空。
// 它不依赖界面，也不生成任何给用户看的文本。
class PersonService {
public:
    // 校验并保存。失败时值不可用，改为读 error。
    Result<Person> submit(std::string_view name, int age);

    void clear();

    const std::vector<Person> &people() const { return m_people; }

private:
    std::vector<Person> m_people;
};

} // namespace day5_2
