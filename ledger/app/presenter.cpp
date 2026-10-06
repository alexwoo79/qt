#include "presenter.h"

namespace ledger {

namespace {

constexpr const char *kIdleText = "还没有记账";
constexpr const char *kClearedText = "已清空";

} // namespace

std::string Presenter::idleStatus() const {
    return kIdleText;
}

std::string Presenter::clearedStatus() const {
    return kClearedText;
}

// 整数分 -> "12.34"。只在这里出现"元"的写法，core 永远只知道分。
std::string Presenter::formatCents(int cents) {
    const bool negative = cents < 0;
    const long long magnitude = negative ? -static_cast<long long>(cents) : cents;

    std::string fraction = std::to_string(magnitude % 100);
    if (fraction.size() < 2)
        fraction.insert(fraction.begin(), '0'); // 5 -> "05"

    return std::string(negative ? "-" : "") + std::to_string(magnitude / 100) + "." + fraction;
}

std::string Presenter::formatEntry(const Entry &entry) const {
    return entry.name + " " + formatCents(entry.amount_cents) + " 元";
}

EntryView Presenter::toView(const Entry &entry) const {
    return EntryView{entry.name, formatCents(entry.amount_cents) + " 元"};
}

std::vector<EntryView> Presenter::toViews(const std::vector<Entry> &entries) const {
    std::vector<EntryView> views;
    views.reserve(entries.size());
    for (const Entry &entry : entries)
        views.push_back(toView(entry));
    return views;
}

std::string Presenter::formatTotal(int totalCents) const {
    return "合计 " + formatCents(totalCents) + " 元";
}

std::string Presenter::addedStatus(const Entry &entry) const {
    return "✅ 已记一笔: " + formatEntry(entry);
}

std::string Presenter::messageFor(const Error &error) const {
    return lookup(error.code, error.key);
}

std::string Presenter::failedStatus(const Error &error) const {
    return "❌ " + messageFor(error);
}

std::string Presenter::lookup(ErrorCode code, const std::string &key) {
    // 先按 key 精确匹配：每个业务域可以有自己更具体的说法。
    if (key == "entry.name.empty")
        return "名称不能为空";
    if (key == "entry.amount.not_positive")
        return "金额必须大于 0";
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

} // namespace ledger
