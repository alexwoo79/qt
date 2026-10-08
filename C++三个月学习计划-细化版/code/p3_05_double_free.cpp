// 例题 3-5：重复释放 —— 同一块内存还两次，堆元数据被破坏
// 编译：clang++ -std=c++17 -g -O0 -fsanitize=address p3_05_double_free.cpp -o p3_05
#include <cstdio>
#include <memory>

int main() {
    printf("-- 正面：所有权写进类型里，就没有第二次释放的机会 --\n");
    {
        auto safe = std::make_unique<int>(2);
        printf("  unique_ptr 管着 %d，作用域结束自动释放一次\n", *safe);
        // delete safe.get();   // 想重复释放得先绕过类型系统：unique_ptr 会再删一次
    }
    printf("  离开作用域，资源恰好释放一次。\n\n");

    printf("-- 反面：裸指针 delete 两次 --\n");
    int *raw = new int(1);
    delete raw;
    fflush(stdout);
    delete raw;              // ← ASan 报 attempting double-free
    return 0;
}
