#include <iostream>

#define LOG(x) std::cout << x << std::endl;

enum Example { VALUE1 = 5, VALUE2, VALUE3 };
int main() {
    Example e = VALUE1;
    LOG(e)
    e = VALUE2;
    LOG(e)
    e = VALUE3;
    LOG(e)

    std::cin.get();
    return 0;
}