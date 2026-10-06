#include "protocol.h"

#include <nlohmann/json.hpp>

#include <cmath>

namespace ledger {

bool decodeAddEntryArgs(const std::string &argsJson, AddEntryArgs &out) {
    // 第三个参数 false = 解析失败返回 discarded 值而不是抛异常。
    const nlohmann::json args = nlohmann::json::parse(argsJson, nullptr, false);
    if (!args.is_array())
        return false;

    if (args.size() > 0 && args[0].is_string())
        out.name = args[0].get<std::string>();

    if (args.size() > 1 && args[1].is_number()) {
        const double yuan = args[1].get<double>();
        // 界面按"元"输入，业务层只认整数"分"。换算是边界的事，
        // 所以只在这一处发生，换算完就用整数往下传，不让浮点数进入 core。
        out.amountCents = static_cast<int>(std::lround(yuan * 100.0));
    }
    return true;
}

std::string encode(const Envelope &envelope) {
    nlohmann::ordered_json entries = nlohmann::ordered_json::array();
    for (const EntryView &entry : envelope.entries)
        entries.push_back({{"name", entry.name}, {"amount", entry.amount}});

    // ordered_json：字段顺序与写入顺序一致，读日志更直观。
    return nlohmann::ordered_json{{"ok", envelope.ok},
                                  {"level", envelope.level},
                                  {"event", envelope.event},
                                  {"status", envelope.status},
                                  {"detail", envelope.detail},
                                  {"error", envelope.error},
                                  {"total", envelope.total},
                                  {"entries", entries}}
        .dump();
}

} // namespace ledger
