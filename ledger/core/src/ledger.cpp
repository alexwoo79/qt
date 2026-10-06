#include "ledger/ledger.h"

#include "text_utils.h"

namespace ledger {

Result<Entry> LedgerService::add(std::string_view name, int amountCents) {
    const std::string trimmed = detail::trim(name);
    if (trimmed.empty())
        return {{}, {ErrorCode::invalid_input, "entry.name.empty", {}}};

    if (amountCents <= 0)
        return {{},
                {ErrorCode::invalid_input, "entry.amount.not_positive", std::to_string(amountCents)}};

    m_entries.push_back(Entry{trimmed, amountCents});
    return {m_entries.back(), {}};
}

void LedgerService::clear() {
    m_entries.clear();
}

int LedgerService::totalCents() const {
    int total = 0;
    for (const Entry &entry : m_entries)
        total += entry.amount_cents;
    return total;
}

} // namespace ledger
