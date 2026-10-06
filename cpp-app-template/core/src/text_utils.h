#pragma once

#include <cctype>
#include <string>
#include <string_view>

// 模块内部工具：放在 src/ 下，不对外暴露（不会进 include/）。
namespace myapp::detail {

inline std::string trim(std::string_view text) {
    std::size_t begin = 0;
    std::size_t end = text.size();
    while (begin < end && std::isspace(static_cast<unsigned char>(text[begin])))
        ++begin;
    while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1])))
        --end;
    return std::string{text.substr(begin, end - begin)};
}

} // namespace myapp::detail
