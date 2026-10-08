// 例题 1-4：虚函数如何改变对象内存（vptr 与虚表）
// 观察点：vptr 在对象最前面；虚表是每个类一张全局表，对象只存一个指针
#include <cstdio>

struct Plain {
    int value;
    void f() { printf("Plain::f()\n"); }
};

struct WithVirtual {
    int value;
    virtual void f() { printf("WithVirtual::f()\n"); }
    virtual void g(int x) { printf("WithVirtual::g(%d)\n", x); }
};

struct Derived : WithVirtual {
    void f() override { printf("Derived::f()\n"); }
};

static void dump_vtable(void *obj, const char *name, int slots) {
    void **vptr = *reinterpret_cast<void ***>(obj);
    printf("%-15s vptr=%p\n", name, (void *)vptr);
    for (int i = 0; i < slots; ++i) printf("    vtable[%d] = %p\n", i, vptr[i]);
}

int main() {
    printf("sizeof(Plain)       = %zu（int + 填充）\n", sizeof(Plain));
    printf("sizeof(WithVirtual) = %zu（int 4 + 填充 4 + vptr 8）\n", sizeof(WithVirtual));
    printf("sizeof(Derived)     = %zu（没加成员，只是换掉表里的函数）\n\n", sizeof(Derived));

    WithVirtual base;
    WithVirtual base2;
    Derived derived;
    dump_vtable(&base, "WithVirtual", 2);
    dump_vtable(&base2, "WithVirtual#2", 2);
    dump_vtable(&derived, "Derived", 2);
    printf("→ 同类对象共用同一张表；子类重写后表里的函数地址不同。\n\n");

    printf("对象前 8 字节的原始值（就是 vptr）：");
    unsigned char *raw = reinterpret_cast<unsigned char *>(&base);
    for (int i = 0; i < 8; ++i) printf("%02x", raw[i]);
    printf("\n\n-- 绕过语法，直接按虚表调用（多态的机械本质）--\n");
    void **vptr = *reinterpret_cast<void ***>(&base);
    auto f0 = reinterpret_cast<void (*)(WithVirtual *)>(vptr[0]);
    auto f1 = reinterpret_cast<void (*)(WithVirtual *, int)>(vptr[1]);
    f0(&base);
    f1(&base, 42);
    return 0;
}
