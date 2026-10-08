// 例题 3-4：释放后使用（野指针）—— 内存已经还给分配器，读到的可能是任意值
// 编译：clang++ -std=c++17 -g -O0 -fsanitize=address p3_04_use_after_free.cpp -o p3_04
#include <cstdio>

int main() {
    int *p = new int(7);
    printf("分配：p=%p 值=%d\n", (void *)p, *p);
    delete p;
    printf("已 delete p，但指针变量还在（这就是野指针）\n");
    fflush(stdout);
    printf("读野指针：*p=%d\n", *p);   // ← ASan 报 heap-use-after-free
    p = nullptr;                      // 好习惯：释放后置空
    if (!p) printf("置空后可以安全判断：p == nullptr\n");
    return 0;
}
