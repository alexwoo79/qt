#pragma once

#include <string>

namespace ledger {

// 业务失败的分类。用 enum class 而不是字符串：写错名字会编译失败。
enum class ErrorCode {
    none,
    invalid_input, // 输入不合法：可预期的失败，用返回值报告
    not_found,
    conflict,
    internal,
};

// 一次业务失败的描述。
//   code   : 失败分类，界面可以据此做粗粒度的统一处理
//   key    : 机器可读的稳定标识，界面可以据此精确反应或做多语言
//   detail : 补充数据（比如出错的原始数值），不是给用户看的句子
struct Error {
    ErrorCode code = ErrorCode::none;
    std::string key;
    std::string detail;

    [[nodiscard]] bool ok() const { return code == ErrorCode::none; }
};

// 业务函数的统一返回形状：要么拿到值，要么拿到失败原因。
// C++23 把等价的类型标准化成了 std::expected<T, E>，届时可以直接替换掉它。
template <typename T>
struct Result {
    T value{};
    Error error{};

    [[nodiscard]] bool ok() const { return error.ok(); }
};

} // namespace ledger
