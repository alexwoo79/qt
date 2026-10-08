// 例题 2-5：shared_ptr 的引用计数、循环引用，以及 weak_ptr 怎么破环
#include <cstdio>
#include <memory>

struct Node {
    int id;
    std::shared_ptr<Node> next;   // 强引用：会推高计数
    std::weak_ptr<Node> back;     // 弱引用：不增加计数
    explicit Node(int i) : id(i) { printf("  构造 Node(%d)\n", id); }
    ~Node() { printf("  析构 Node(%d)\n", id); }
};

static void countDemo() {
    printf("[引用计数变化]\n");
    auto a = std::make_shared<Node>(1);
    printf("  make_shared 后 use_count=%ld\n", a.use_count());
    {
        auto b = a;
        printf("  拷贝一份后 use_count=%ld\n", a.use_count());
    }
    printf("  副本销毁后 use_count=%ld\n", a.use_count());
    auto c = std::move(a);
    printf("  move 之后：源=%s，新=%ld\n", a ? "非空" : "空", c.use_count());
}

static void cycleLeak() {
    printf("\n[循环引用：两个 shared_ptr 互指 → 谁都释放不了]\n");
    auto x = std::make_shared<Node>(2);
    auto y = std::make_shared<Node>(3);
    x->next = y;
    y->next = x;
    printf("  离开作用域前：use_count x=%ld y=%ld\n", x.use_count(), y.use_count());
    printf("  → 你会看到下面没有析构输出，说明对象泄漏了（ASan 能抓到）\n");
}

static void fixedCycle() {
    printf("\n[用 weak_ptr 破环：back 只是观察者]\n");
    auto x = std::make_shared<Node>(4);
    auto y = std::make_shared<Node>(5);
    x->next = y;
    y->back = x;  // 弱引用不推高计数
    printf("  离开作用域前：x.use_count=%ld y.use_count=%ld\n", x.use_count(), y.use_count());
    if (auto locked = y->back.lock()) printf("  弱引用能临时提升为强引用，看到 Node(%d)\n", locked->id);
}

int main() {
    countDemo();
    cycleLeak();
    printf("  （上面没有 Node(2)/Node(3) 的析构 —— 计数永远不归零）\n");
    fixedCycle();
    printf("\nNode(4)/Node(5) 会在离开作用域时正常析构。\n");
    return 0;
}
