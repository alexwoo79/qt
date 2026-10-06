#pragma once

#include "myapp/result.h"

#include <string>
#include <string_view>
#include <vector>

namespace myapp {

// 一条记录的业务数据。这里只存事实，不存任何展示用的文本。
//
// 换成你自己的业务时，把这个结构体换成你的实体（订单、设备、联系人……），
// 文件名和类名跟着一起改。第一版尽量保持"一个类型一个文件"。
struct Task {
    std::string title;
};

// 待办列表的业务逻辑：校验、保存、清空。
// 它不依赖界面，也不生成任何给用户看的文本。
class TaskService {
public:
    // 校验并保存。失败时值不可用，改为读 error。
    Result<Task> add(std::string_view title);

    void clear();

    const std::vector<Task> &tasks() const { return m_tasks; }

private:
    std::vector<Task> m_tasks;
};

} // namespace myapp
