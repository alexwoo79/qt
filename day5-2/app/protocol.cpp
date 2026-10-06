#include "protocol.h"

#include <nlohmann/json.hpp>

namespace day5_2 {

bool decodeSubmitArgs(const std::string &argsJson, SubmitArgs &out) {
    // 第三个参数 false = 解析失败返回 discarded 值而不是抛异常。
    const nlohmann::json args = nlohmann::json::parse(argsJson, nullptr, false);
    if (!args.is_array())
        return false;

    if (args.size() > 0 && args[0].is_string())
        out.name = args[0].get<std::string>();
    if (args.size() > 1 && args[1].is_number())
        out.age = args[1].get<int>();
    return true;
}

std::string encode(const Envelope &envelope) {
    // ordered_json：字段顺序与写入顺序一致，读日志更直观。
    return nlohmann::ordered_json{{"ok", envelope.ok},
                                  {"level", envelope.level},
                                  {"event", envelope.event},
                                  {"status", envelope.status},
                                  {"detail", envelope.detail},
                                  {"error", envelope.error},
                                  {"messages", envelope.messages}}
        .dump();
}

} // namespace day5_2
