#include <iostream> // 提供 std::cin 和 std::cout 等标准输入输出功能。

int main() {
    int sum = 0,
        value =
            0; // sum 保存累计和；value 保存每次读入的整数，二者初始值都为 0。
    while (std::cin >> value) {
        sum += value; // 成功读入一个整数后，将它加到累计和 sum 中。
    }
    std::cout << "Sum is : " << sum
              << std::endl; // 输出当前累计和；endl 同时换行并刷新输出缓冲区。

    return 0; // 返回 0，表示程序正常结束。 输入ctr+d 结束输入。
}