#pragma once

#include "ledger/ledger.h"
#include "ledger/result.h"
#include "protocol.h"

#include <string>
#include <vector>

namespace ledger {

// 展示层：整个项目里唯一生成面向用户文本的地方。
//
// core 只给"事实"（整数分、错误码 + key），这里负责把事实变成人话。
// 换语言、换图标、把"元"改成"¥"，都只改这一个类。
class Presenter {
public:
    std::string idleStatus() const;
    std::string clearedStatus() const;
    std::string addedStatus(const Entry &entry) const;
    std::string failedStatus(const Error &error) const;

    // 把一条记录格式化成列表里显示的样子："午餐 12.34 元"
    std::string formatEntry(const Entry &entry) const;

    // 列表用的展示数据：名称 + 金额文本，界面自己排版。
    EntryView toView(const Entry &entry) const;
    std::vector<EntryView> toViews(const std::vector<Entry> &entries) const;

    // 合计："合计 13.00 元"
    std::string formatTotal(int totalCents) const;

    // 错误码 / 错误 key -> 中文说明。
    std::string messageFor(const Error &error) const;

private:
    static std::string formatCents(int cents);
    static std::string lookup(ErrorCode code, const std::string &key);
};

} // namespace ledger
