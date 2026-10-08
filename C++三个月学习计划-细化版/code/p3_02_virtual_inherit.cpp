// 例题 3-2：菱形继承与虚继承 —— 两份 Base 还是一份 Base
#include <cstdio>

struct Base {
    int value = 10;
    virtual void show() { printf("Base::show() value=%d\n", value); }
    virtual ~Base() = default;
};

// 不用虚继承：D 里有两份 Base
struct Left : Base {};
struct Right : Base {};
struct Diamond : Left, Right {};

// 用虚继承：只剩一份 Base，编译器插入 vbptr/偏移表来定位它
struct VLeft : virtual Base {};
struct VRight : virtual Base {};
struct VShape : VLeft, VRight {
    void show() override { printf("VShape::show() value=%d（唯一一份 Base）\n", value); }
};

int main() {
    printf("sizeof(Base)=%zu sizeof(Left)=%zu sizeof(Diamond)=%zu\n",
           sizeof(Base), sizeof(Left), sizeof(Diamond));
    printf("→ Diamond 里有两份 Base（2 个 vptr + 2 个 int）\n");
    printf("sizeof(VLeft)=%zu sizeof(VRight)=%zu sizeof(VShape)=%zu\n",
           sizeof(VLeft), sizeof(VRight), sizeof(VShape));
    printf("→ 虚继承后只有一份 Base，多出来的是指向公共 Base 的 vbptr\n\n");

    Diamond d;
    printf("[非虚继承]\n");
    d.Left::value = 1;
    d.Right::value = 2;
    printf("  d.Left::value=%d 落在 %p\n", d.Left::value, (void *)&d.Left::value);
    printf("  d.Right::value=%d 落在 %p\n", d.Right::value, (void *)&d.Right::value);
    printf("  两个 Base 子对象地址相差 %ld 字节，写 d.value 会报二义性编译错误。\n\n",
           (long)((char *)&d.Right::value - (char *)&d.Left::value));

    VShape s;
    printf("[虚继承]\n");
    s.value = 99;  // 只有一份，不需要限定名
    printf("  &s.value = %p，VLeft* 与 VRight* 指向的公共 Base 相同：%s\n",
           (void *)&s.value,
           (void *)static_cast<Base *>(static_cast<VLeft *>(&s)) ==
                   (void *)static_cast<Base *>(static_cast<VRight *>(&s))
               ? "是"
               : "否");
    printf("  通过左路指针调用虚函数：\n");
    static_cast<VLeft *>(&s)->show();

    printf("\n[内存布局速查]\n");
    printf("  非虚菱形：Diamond{ Left{ vptr, Base{...}? }, Right{...} } → 2 份数据，改一处另一处不动\n");
    printf("  虚菱形：  VShape{ vbptr..., 公共 Base 一份 } → 只有一份数据，代价是每次访问多一次间接寻址\n");
    return 0;
}
