// 例题 2-4：unique_ptr —— 编译期就禁止"共享"，对齐 Rust 的所有权直觉
#include <cstdio>
#include <memory>
#include <utility>
#include <vector>

struct Resource {
    int id;
    explicit Resource(int i) : id(i) { printf("  构造 Resource(%d)\n", id); }
    ~Resource() { printf("  析构 Resource(%d)\n", id); }
};

static void consume(std::unique_ptr<Resource> p) {  // 按值接收 = 拿走所有权
    printf("  consume 里看到的 id=%d\n", p->id);
}   // 离开函数即析构

static void observe(const std::unique_ptr<Resource> &p) {  // 借用，不转移
    printf("  observe 里看到的 id=%d（所有权还在调用方）\n", p->id);
}

int main() {
    printf("-- 构造与自动释放 --\n");
    {
        auto p = std::make_unique<Resource>(1);
        observe(p);
        printf("  离开作用域 → 自动 delete\n");
    }

    printf("\n-- 移动：所有权转移，源指针变空（Rust 的 move 同款语义）--\n");
    {
        auto owner = std::make_unique<Resource>(2);
        printf("  move 前 owner=%p\n", (void *)owner.get());
        auto thief = std::move(owner);
        printf("  move 后 owner=%p（空） thief=%p\n", (void *)owner.get(), (void *)thief.get());
        if (!owner) printf("  对空 unique_ptr 解引用是 UB，只能先判空。\n");
    }

    printf("\n-- 传参把所有权交出去 --\n");
    consume(std::make_unique<Resource>(3));

    printf("\n-- 放进容器：只移动，指针稳定 --\n");
    {
        std::vector<std::unique_ptr<Resource>> pool;
        pool.reserve(3);
        for (int i = 4; i <= 6; ++i) pool.push_back(std::make_unique<Resource>(i));
        printf("  容器里 %zu 个对象，析构顺序与构造相反：\n", pool.size());
    }

    printf("\n-- 数组形式与自定义删除器 --\n");
    {
        auto arr = std::make_unique<int[]>(4);
        arr[0] = 42;
        printf("  unique_ptr<int[]> arr[0]=%d（析构用 delete[]）\n", arr[0]);
        auto closer = std::unique_ptr<FILE, int (*)(FILE *)>(fopen("/dev/null", "r"), &fclose);
        printf("  自定义删除器：FILE* = %s\n", closer ? "已打开" : "打开失败");
    }
    printf("\n全部作用域结束，所有资源都已释放。\n");
    return 0;
}
