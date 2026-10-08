// 例题 3-3：堆越界写 —— 编译器不拦，ASan 一抓一个准
// 编译：clang++ -std=c++17 -g -O0 -fsanitize=address p3_03_overflow.cpp -o p3_03
#include <cstdio>

int main() {
    int *arr = new int[4];
    for (int i = 0; i < 4; ++i) arr[i] = i;
    printf("正常写完 4 个元素：arr[3]=%d\n", arr[3]);
    printf("下面故意越界写 arr[4]……\n");
    fflush(stdout);
    arr[4] = 99;              // ← 越界：ASan 会在这里报 heap-buffer-overflow
    printf("这行几乎不会执行到：%d\n", arr[4]);
    delete[] arr;
    return 0;
}
