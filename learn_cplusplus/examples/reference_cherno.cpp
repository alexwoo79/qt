#include <iostream>

#define LOG(x) std::cout << x << std::endl;

void Increment(int& value) {
    value++;
}
int main() {

    int a = 5;
    int b = 8;
    int& ref = a;// ref 是 a 的别名
    ref = b;       // 不是让 ref 改为引用 b，而是把 b 的值赋给 a

    // Increment(a);
    LOG(a);
    LOG(b);
    LOG(ref);


    std::cin.get();
}