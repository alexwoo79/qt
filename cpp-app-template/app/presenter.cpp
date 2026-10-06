#include "presenter.h"

namespace myapp::app {

namespace {

constexpr const char *kIdleText = "还没有任务";
constexpr const char *kClearedText = "已清空";

} // namespace

std::string Presenter::idleStatus() const {
    return kIdleText;
}

std::string Presenter::clearedStatus() const {
    return kClearedText;
}

std::string Presenter::formatTask(const Task &task) const {
    return task.title;
}

std::vector<std::string> Presenter::formatTasks(const std::vector<Task> &tasks) const {
    std::vector<std::string> lines;
    lines.reserve(tasks.size());
    for (const Task &task : tasks)
        lines.push_back(formatTask(task));
    return lines;
}

std::string Presenter::addedStatus(const Task &task) const {
    return "✅ 已添加: " + formatTask(task);
}

std::string Presenter::messageFor(const Error &error) const {
    return lookup(error.code, error.key);
}

std::string Presenter::failedStatus(const Error &error) const {
    return "❌ " + messageFor(error);
}

std::string Presenter::lookup(ErrorCode code, const std::string &key) {
    // 先按 key 精确匹配：每个业务域可以有自己更具体的说法。
    if (key == "task.title.empty")
        return "标题不能为空";
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

} // namespace myapp::app
