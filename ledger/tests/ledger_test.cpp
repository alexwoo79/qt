// 业务测试：不启动界面，直接调用 core 层的函数。
// 没有引入测试框架——assert 风格的检查就够小项目用了，少一个依赖少一件事。
#include "ledger/ledger.h"

#include <iostream>
#include <string>

namespace {

int g_failures = 0;

void check(bool condition, const std::string &what) {
    if (condition) {
        std::cout << "ok   " << what << '\n';
    } else {
        std::cout << "FAIL " << what << '\n';
        ++g_failures;
    }
}

void empty_name_is_rejected() {
    ledger::LedgerService service;
    const auto result = service.add("   ", 1234);

    check(!result.ok(), "只有空白的名称被拒绝");
    check(result.error.key == "entry.name.empty", "失败原因是 entry.name.empty");
    check(result.error.code == ledger::ErrorCode::invalid_input, "失败分类是 invalid_input");
    check(service.entries().empty(), "被拒绝的输入不会写进账本");
}

void non_positive_amount_is_rejected() {
    ledger::LedgerService service;

    check(!service.add("午餐", 0).ok(), "金额 0 被拒绝");
    check(!service.add("午餐", -1).ok(), "负数金额被拒绝");
    check(service.entries().empty(), "越界的输入一条都没写进去");
}

void one_cent_is_accepted() {
    ledger::LedgerService service;

    check(service.add("最小额", 1).ok(), "1 分钱的记录可以记");
    check(service.totalCents() == 1, "合计是 1 分");
}

void name_is_trimmed_and_amount_kept_as_cents() {
    ledger::LedgerService service;
    const auto result = service.add("  午餐  ", 1234);

    check(result.ok(), "合法输入通过");
    check(result.value.name == "午餐", "名称去掉了首尾空白");
    check(result.value.amount_cents == 1234, "金额按分原样保留");
    check(service.entries().size() == 1, "账本里有一条记录");
}

void total_sums_all_entries() {
    ledger::LedgerService service;
    service.add("午餐", 1234); // 12.34 元
    service.add("咖啡", 66);   // 0.66 元

    check(service.entries().size() == 2, "两笔都记上了");
    check(service.totalCents() == 1300, "合计正好 13.00 元，没有浮点误差");
}

void clear_resets_entries_and_total() {
    ledger::LedgerService service;
    service.add("午餐", 1234);
    service.clear();

    check(service.entries().empty(), "清空之后账本为空");
    check(service.totalCents() == 0, "清空之后合计归零");
}

} // namespace

int main() {
    empty_name_is_rejected();
    non_positive_amount_is_rejected();
    one_cent_is_accepted();
    name_is_trimmed_and_amount_kept_as_cents();
    total_sums_all_entries();
    clear_resets_entries_and_total();

    if (g_failures == 0) {
        std::cout << "\n全部通过\n";
        return 0;
    }
    std::cout << "\n失败 " << g_failures << " 项\n";
    return 1;
}
