#include <iostream> // 提供 std::cin 和 std::cout 等标准输入输出功能。

int main() {
    int currVal = 0, val = 0;  // currVal 保存当前正在统计的整数；val
                               // 保存下一次从输入中读到的整数。
    if (std::cin >> currVal) { // 先尝试读第一个整数；成功才进入统计。>>
                               // 会跳过前导空白，读一个整数。
        int cnt = 1; // 第一个整数已读入，因此当前连续整数段的数量从 1 开始。
        while (
            std::cin >>
            val) { // 每次再读一个整数；空格、制表符、换行都是分隔符，不会被读入整数。
            if (val ==
                currVal) { // 新读到的整数与当前段相同，说明仍属于同一段。
                ++cnt;     // 当前连续整数段的计数加一。
            } else {
                // 数值改变，当前段结束：输出刚才统计的 currVal
                // 及其连续出现次数。
                std::cout << currVal << " occurs " << cnt << " times"
                          << std::endl; // std::endl 输出换行并刷新输出缓冲区。
                currVal = val;          // 把新读到的整数作为下一段要统计的值。
                cnt = 1; // 新段至少包含刚读到的这个整数，所以计数重置为 1。
            }
        }
        // 输入结束或读取失败时，循环中尚未输出的最后一段需要在这里输出。
        std::cout << currVal << " occurs " << cnt << " times" << std::endl;
    }

    return 0; // 返回 0，表示程序正常结束。
}