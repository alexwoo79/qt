#pragma once

#include <string>
#include <vector>

namespace ledger {

// 前端 -> 后端的参数。具名字段，比位置数组 [{0},{1}] 更耐改。
struct AddEntryArgs {
    std::string name;
    int amountCents = 0; // 界面输入的是"元"，进业务层之前已经换算成整数"分"
};

// 解析 addEntry(name, amount) 传来的参数数组。失败返回 false，不抛异常
// （异常从 webview 的 C 回调里逃出去是不安全的）。
bool decodeAddEntryArgs(const std::string &argsJson, AddEntryArgs &out);

// 信封里"一条记录"的形状。界面直接拿去排版：名称靠左、金额靠右。
// 金额是展示层算好的文本，界面不需要再做单位换算。
// 加业务字段时，这里跟着加——协议就是靠这个结构体长出来的。
struct EntryView {
    std::string name;
    std::string amount;
};

// 线上协议信封：字段名就是前后端契约，改这里等于改协议。
//   level : "info" 中性 / "ok" 成功 / "error" 失败 —— 界面据此决定样式，
//           不再靠嗅探 ✅❌ 这类文本前缀。
//   error : 失败时的机器可读 key，成功后为空。
//   total : 合计金额（已格式化），由展示层算好给界面直接用。
struct Envelope {
    bool ok = false;
    std::string level;
    std::string event;
    std::string status;
    std::string detail;
    std::string error;
    std::string total;
    std::vector<EntryView> entries;
};

std::string encode(const Envelope &envelope);

} // namespace ledger
