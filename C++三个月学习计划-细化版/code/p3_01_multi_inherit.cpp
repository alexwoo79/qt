// 例题 3-1：多继承的内存布局 —— 一个对象里为什么会有两个 vptr
#include <cstdio>

struct A {
    int a = 1;
    virtual void fa() { printf("A::fa()\n"); }
    virtual ~A() = default;
};

struct B {
    int b = 2;
    virtual void fb() { printf("B::fb()\n"); }
    virtual ~B() = default;
};

struct C : A, B {
    int c = 3;
    void fa() override { printf("C::fa()（重写 A 的）\n"); }
    void fb() override { printf("C::fb()（重写 B 的）\n"); }
    ~C() override = default;
};

int main() {
    printf("sizeof(A)=%zu sizeof(B)=%zu sizeof(C)=%zu\n", sizeof(A), sizeof(B), sizeof(C));
    printf("→ C = A 子对象(vptr+int, 16) + B 子对象(vptr+int, 16) + int c + 填充\n\n");

    C obj;
    A *pa = &obj;
    B *pb = &obj;
    C *pc = &obj;
    printf("同一个对象，三个指针指向不同的位置：\n");
    printf("  pc = %p（对象首地址）\n", (void *)pc);
    printf("  pa = %p（A 子对象，偏移 %ld）\n", (void *)pa, (long)((char *)pa - (char *)pc));
    printf("  pb = %p（B 子对象，偏移 %ld）\n", (void *)pb, (long)((char *)pb - (char *)pc));

    printf("\n两个子对象各有一张虚表：\n");
    printf("  A 子对象的 vptr = %p\n", *reinterpret_cast<void **>(pa));
    printf("  B 子对象的 vptr = %p\n", *reinterpret_cast<void **>(pb));

    printf("\n通过不同指针调用，都是虚调用，各自查各自的表：\n");
    pa->fa();
    pb->fb();

    printf("\n-- 指针调整是编译器做的，不是免费的 --\n");
    B *fromC = static_cast<B *>(pc);        // 正确：C 明确继承 B，编译器自动偏移
    B *wrong = reinterpret_cast<B *>(pa);   // 危险：按字节重解释，地址没有偏移
    B *cross = dynamic_cast<B *>(pa);       // 运行时跨类转换（A 多态才允许）
    printf("  static_cast<B*>(pc)      = %p（正确，从对象首地址偏移到 B 子对象）\n", (void *)fromC);
    printf("  reinterpret_cast<B*>(pa) = %p（错，等于把 A 子对象当 B 用 → UB）\n", (void *)wrong);
    printf("  dynamic_cast<B*>(pa)     = %p（正确，代价是运行时检查）\n", (void *)cross);

    printf("\n-- 用基类指针 delete：虚析构保证完整析构 --\n");
    A *heap = new C();
    printf("  delete 前先看 pb 偏移：B* p = %p\n", (void *)dynamic_cast<B *>(heap));
    delete heap;
    return 0;
}
