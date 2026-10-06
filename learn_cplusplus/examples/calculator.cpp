#include <iostream>
#include <cmath>
#include <string>
#include <limits>

int main() {
    double a = 0.0, b = 0.0, result = 0.0;
    char op;

    std::cout << "===== 简单计算器 =====\n";
    std::cout << "支持的运算符: + - * / % ^ s c t l e\n";
    std::cout << "  s = sin, c = cos, t = tan\n";
    std::cout << "  l = log (自然对数), e = exp (e 的幂)\n";
    std::cout << "  单目运算 (s/c/t/l/e) 只需输入一个数\n\n";

    std::cout << "请输入运算符: ";
    std::cin >> op;

    // 判断是否为单目运算
    bool isUnary = (op == 's' || op == 'c' || op == 't' ||
                    op == 'l' || op == 'e');

    if (isUnary) {
        std::cout << "请输入一个数字: ";
        std::cin >> a;

        switch (op) {
            case 's': result = std::sin(a); break;
            case 'c': result = std::cos(a); break;
            case 't': result = std::tan(a); break;
            case 'l':
                if (a <= 0) {
                    std::cerr << "错误: log 的参数必须大于 0\n";
                    return 1;
                }
                result = std::log(a);
                break;
            case 'e': result = std::exp(a); break;
        }
    } else {
        std::cout << "请输入两个数字 (用空格分隔): ";
        std::cin >> a >> b;

        switch (op) {
            case '+': result = a + b; break;
            case '-': result = a - b; break;
            case '*': result = a * b; break;
            case '/':
                if (b == 0) {
                    std::cerr << "错误: 除数不能为 0\n";
                    return 1;
                }
                result = a / b;
                break;
            case '%':
                result = std::fmod(a, b);
                break;
            case '^':
                result = std::pow(a, b);
                break;
            default:
                std::cerr << "错误: 未知运算符 '" << op << "'\n";
                return 1;
        }
    }

    std::cout << "结果: " << result << "\n";
    return 0;
}