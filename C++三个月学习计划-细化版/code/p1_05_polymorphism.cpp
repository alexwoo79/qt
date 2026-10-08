// 例题 1-5：多态 = 运行时通过 vptr 查表；非虚函数 = 编译期按指针类型绑定
#include <cstdio>

struct Base {
    virtual void speak() { printf("Base::speak()\n"); }
    void name() { printf("Base::name()    ← 非虚，看指针的静态类型\n"); }
    virtual ~Base() { printf("~Base()\n"); }
};

struct Derived : Base {
    void speak() override { printf("Derived::speak()\n"); }
    void name() { printf("Derived::name() ← 非虚，看指针的静态类型\n"); }
    virtual void extra() { printf("Derived::extra() ← 新增虚函数，追加在表尾\n"); }
    ~Derived() override { printf("~Derived()\n"); }
};

int main() {
    Derived d;
    Base *pb = &d;
    Derived *pd = &d;
    printf("&d=%p  pb=%p  pd=%p（单继承下三者地址相同）\n", (void *)&d, (void *)pb, (void *)pd);

    printf("\n-- 虚函数：运行时按对象真实类型查表 --\n");
    pb->speak();
    pd->speak();

    printf("\n-- 非虚函数：编译期按指针类型绑定 --\n");
    pb->name();
    pd->name();

    printf("\n-- 打印两张不同的虚表 --\n");
    Base fake;
    void **base_vptr = *reinterpret_cast<void ***>(&fake);
    void **derived_vptr = *reinterpret_cast<void ***>(pd);
    printf("Base 对象 vptr=%p，Derived 对象 vptr=%p\n", (void *)base_vptr, (void *)derived_vptr);
    printf("speak 槽位：Base=%p  Derived=%p\n", base_vptr[0], derived_vptr[0]);

    printf("\n-- 虚析构：用基类指针 delete 必须靠它 --\n");
    Base *heap = new Derived();
    delete heap;
    printf("（~Base 不是 virtual 时，上面只会打印 ~Base，Derived 的成员就泄漏了）\n");
    return 0;
}
