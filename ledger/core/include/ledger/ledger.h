#pragma once

#include "ledger/result.h"

#include <string>
#include <string_view>
#include <vector>

namespace ledger {

// 一笔开销。这里只存事实，不存任何展示用的文本。
//
// 金额用"分"存成整数：钱不能用浮点数表示（0.1 + 0.2 != 0.3 那套问题）。
// "元"只是给人看的写法，属于展示层，由 app/presenter.cpp 负责换算。
struct Entry {
    std::string name;
    int amount_cents = 0;
};

// 记账的业务逻辑：校验、保存、求和、清空。
// 它不依赖界面，也不生成任何给用户看的文本。
class LedgerService {
public:
    // 校验并记一笔。失败时值不可用，改为读 error。
    Result<Entry> add(std::string_view name, int amountCents);

    void clear();

    const std::vector<Entry> &entries() const { return m_entries; }

    // 合计：这是业务规则，不是界面的事，所以放在 core 里。
    int totalCents() const;

private:
    std::vector<Entry> m_entries;
};

} // namespace ledger
