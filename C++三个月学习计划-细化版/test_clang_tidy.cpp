#include <iostream>
#include <string>
#include <vector>

int main() {
    // 触发未初始化变量告警
    int a;
    std::cout << a << '\n';

    // 触发冗余深拷贝提示
    std::string origin = "hello_test_sso";

    // 触发悬垂指针风险提示
    std::vector<int> *vec_ptr;
    {
        std::vector<int> temp_vec = {1, 2, 3};
        vec_ptr = &temp_vec;
    }
    // 出作用域后访问已销毁的vector
    for (auto num : *vec_ptr) {
        std::cout << num << '\n';
    }
    return 0;
}