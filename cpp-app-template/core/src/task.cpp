#include "myapp/task.h"

#include "text_utils.h"

namespace myapp {

Result<Task> TaskService::add(std::string_view title) {
    const std::string trimmed = detail::trim(title);
    if (trimmed.empty())
        return {{}, {ErrorCode::invalid_input, "task.title.empty", {}}};

    m_tasks.push_back(Task{trimmed});
    return {m_tasks.back(), {}};
}

void TaskService::clear() {
    m_tasks.clear();
}

} // namespace myapp
