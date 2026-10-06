#pragma once

#include "myapp/result.h"
#include "myapp/task.h"

#include <string>
#include <vector>

namespace myapp::app {

// 展示层：整个项目里唯一生成面向用户文本的地方。
//
// core 只给"事实"（成功的数据，或错误码 + key），这里负责把事实变成人话。
// 换语言、换图标、换成英文界面，都只改这一个类。
class Presenter {
public:
    std::string idleStatus() const;
    std::string clearedStatus() const;
    std::string addedStatus(const Task &task) const;
    std::string failedStatus(const Error &error) const;

    // 把一条记录格式化成列表里显示的样子。
    std::string formatTask(const Task &task) const;
    std::vector<std::string> formatTasks(const std::vector<Task> &tasks) const;

    // 错误码 / 错误 key -> 中文说明。
    std::string messageFor(const Error &error) const;

private:
    static std::string lookup(ErrorCode code, const std::string &key);
};

} // namespace myapp::app
