#pragma once

#include <string>
#include <vector>

namespace day5_2 {

// 前端 -> 后端的参数。具名字段，比位置数组 [{0},{1}] 更耐改。
struct SubmitArgs {
    std::string name;
    int age = 0;
};

// 解析 submitEntry(name, age) 传来的参数数组。失败返回 false，不抛异常
// （异常从 webview 的 C 回调里逃出去是不安全的）。
bool decodeSubmitArgs(const std::string &argsJson, SubmitArgs &out);

// 线上协议信封：字段名就是前后端契约，改这里等于改协议。
//   level : "info" 中性 / "ok" 成功 / "error" 失败 —— 界面据此决定样式，
//           不再靠嗅探 ✅❌ 这类文本前缀。
//   error : 失败时的机器可读 key，成功后为空。
struct Envelope {
    bool ok = false;
    std::string level;
    std::string event;
    std::string status;
    std::string detail;
    std::string error;
    std::vector<std::string> messages;
};

std::string encode(const Envelope &envelope);

} // namespace day5_2
