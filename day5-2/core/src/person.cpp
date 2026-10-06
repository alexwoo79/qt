#include "day5_2/person.h"

#include "text_utils.h"

#include <string>

namespace day5_2 {

namespace {

constexpr int kMinAge = 1;
constexpr int kMaxAge = 150;

} // namespace

Result<Person> PersonService::submit(std::string_view name, int age) {
    const std::string trimmed = detail::trim(name);
    if (trimmed.empty())
        return {{}, {ErrorCode::invalid_input, "person.name.empty", {}}};

    if (age < kMinAge || age > kMaxAge)
        return {{}, {ErrorCode::invalid_input, "person.age.out_of_range", std::to_string(age)}};

    m_people.push_back(Person{trimmed, age});
    return {m_people.back(), {}};
}

void PersonService::clear() {
    m_people.clear();
}

} // namespace day5_2
