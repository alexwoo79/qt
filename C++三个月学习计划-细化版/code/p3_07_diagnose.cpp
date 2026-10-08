// 例题 3-7：不用调试器，靠"打印地址 + 看字节"把问题缩小到一行
// 三个现场：对象切片、悬空引用、容器扩容后旧引用失效
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

static void hexdump(const void *data, std::size_t bytes, const char *label) {
    const unsigned char *p = static_cast<const unsigned char *>(data);
    printf("%-16s @%p : ", label, data);
    for (std::size_t i = 0; i < bytes; ++i) printf("%02x ", p[i]);
    printf("\n");
}

// ---------------- 现场 1：对象切片（不崩，但行为悄悄变了）----------------
struct Base {
    int id;
    explicit Base(int i) : id(i) {}
    virtual const char *who() const { return "Base"; }
    virtual ~Base() = default;
};

struct Derived : Base {
    char extra;  // 子类新增的成员
    explicit Derived(int i) : Base(i), extra('D') {}
    const char *who() const override { return "Derived"; }
};

static void byValue(Base b) {          // ← 按值传：只复制 Base 那部分
    printf("  byValue 收到的对象：sizeof=%zu vptr=%p who()=%s\n",
           sizeof(b), *reinterpret_cast<void **>(&b), b.who());
}

static void byRef(Base &b) {
    printf("  byRef  收到的对象：sizeof=%zu vptr=%p who()=%s\n",
           sizeof(b), *reinterpret_cast<void **>(&b), b.who());
}

int main() {
    printf("=== 现场 1：对象切片 ===\n");
    Derived d(7);
    printf("  原始对象：sizeof=%zu vptr=%p\n", sizeof(d), *reinterpret_cast<void **>(&d));
    hexdump(&d, sizeof(d), "Derived 原始");
    byRef(d);     // 引用：对象没被复制，虚调用仍然正确
    byValue(d);   // 值传递：Derived 被"切成"Base，extra 丢了、虚表也换成 Base 的
    printf("  → 定位手法：打印 vptr。两个 vptr 不同，就说明对象被切片了。\n");
    printf("  → 修法：接口一律按引用/指针传递，或显式 clone 成 unique_ptr。\n\n");

    printf("=== 现场 2：悬空引用 ===\n");
    const std::string *dangling = nullptr;
    {
        std::string local = "临时字符串";
        dangling = &local;
        printf("  作用域内 local @%p 内容=\"%s\"\n", (const void *)&local, local.c_str());
    }
    printf("  local 已销毁，但我们还拿着地址 %p\n", (const void *)dangling);
    printf("  → 继续解引用就是 UB：内容可能看起来是对的，也可能乱码，随机性正是它可怕的地方\n");
    printf("  → 修法：用 shared_ptr/unique_ptr 表达所有权，或复制一份数据\n\n");

    printf("=== 现场 3：容器扩容后旧引用失效 ===\n");
    std::vector<std::string> names{"a", "b"};
    const std::string &first = names.front();
    printf("  扩容前 names.data()=%p，first 引用地址 %p\n",
           (const void *)names.data(), (const void *)&first);
    for (int i = 0; i < 20; ++i) names.push_back("x");
    printf("  扩容后 names.data()=%p，capacity=%zu\n",
           (const void *)names.data(), names.capacity());
    printf("  → 数据已经搬家，first 指向的是旧地址（悬空）\n");
    printf("  → 修法：要么先 reserve 足够容量，要么用下标/迭代器重新取\n\n");

    printf("=== 排坑四步法（把大问题切成一行）===\n");
    printf("  1. 复现：写出 20 行以内的最小例子，稳定触发\n");
    printf("  2. 观测：打印关键地址、sizeof、vptr、size/capacity、引用计数\n");
    printf("  3. 对比：正常路径 vs 异常路径的字节差异（hexdump 对照）\n");
    printf("  4. 验证：用 ASan/UBSan 复跑，确认修完之后不再报错\n");
    return 0;
}
