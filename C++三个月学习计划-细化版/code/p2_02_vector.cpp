// 例题 2-2：vector 的连续内存、扩容倍数、迭代器/指针失效
#include <cstdio>
#include <vector>

struct Tracked {
    int value;
    static int alive;
    static int copies;
    static int moves;
    explicit Tracked(int v) : value(v) { ++alive; }
    Tracked(const Tracked &other) : value(other.value) { ++alive; ++copies; }
    Tracked(Tracked &&other) noexcept : value(other.value) { ++alive; ++moves; }
    ~Tracked() { --alive; }
};
int Tracked::alive = 0;
int Tracked::copies = 0;
int Tracked::moves = 0;

int main() {
    printf("-- 基本类型：扩容时 size/capacity/地址怎么变 --\n");
    std::vector<int> v;
    const void *last = nullptr;
    for (int i = 0; i < 17; ++i) {
        v.push_back(i);
        if (v.data() != last) {
            printf("push_back(%2d) → size=%2zu capacity=%2zu data=%p  ← 发生重分配\n",
                   i, v.size(), v.capacity(), (const void *)v.data());
            last = v.data();
        }
    }
    printf("容量序列大致是 1,2,4,8,16,32：libc++ 按 2 倍增长，均摊 O(1)。\n\n");

    printf("-- 元素是自定义类型：搬家的代价能被看见 --\n");
    {
        std::vector<Tracked> items;
        for (int i = 0; i < 8; ++i) {
            items.emplace_back(i);
        }
        printf("插入 8 个元素后：存活=%d 拷贝=%d 移动=%d\n",
               Tracked::alive, Tracked::copies, Tracked::moves);
        printf("→ 扩容时用移动（Tracked 有 noexcept 移动构造），拷贝次数为 0 才是合格实现。\n");
    }
    printf("vector 离开作用域后：存活=%d（全部析构，没有泄漏）\n\n", Tracked::alive);

    printf("-- 迭代器/指针失效 --\n");
    std::vector<int> w{1, 2, 3};
    const int *p = w.data();
    printf("扩容前 data=%p\n", (const void *)p);
    w.push_back(4);
    printf("push_back 后 data=%p，旧指针 %s\n", (const void *)w.data(),
           p == w.data() ? "仍然有效" : "已悬空（继续解引用就是 UB，ASan 会抓到）");
    w.reserve(64);
    printf("reserve 之后 data=%p，之前那个指针同样失效\n", (const void *)w.data());
    printf("\n结论：只要发生重分配，所有指向元素的指针/引用/迭代器全部作废。\n");
    return 0;
}
