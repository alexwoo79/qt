// 例题 1-1：空类、基础类型、对齐、vptr 到底各占多少字节
// 观察点：sizeof 是"类型合同"，不是"实际用了多少内存"
#include <cstddef>//这个库提供了 size_t 类型
#include <cstdio>//这个库提供了 printf 函数

struct Empty {};                    // 空类：编译器必须给 1 字节占位
struct OneChar { char c; };
struct CharInt { char c; int i; };  // 1 + 3 填充 + 4 = 8
struct IntChar { int i; char c; };  // 4 + 1 + 3 尾部填充 = 8
struct HasFunction { void f(); };   // 非虚成员函数不占对象内存
struct HasVirtual { virtual void f(); };

int main() {
    printf("sizeof(Empty)        = %zu\n", sizeof(Empty));//z：表示参数类型是 size_t
    printf("sizeof(OneChar)      = %zu\n", sizeof(OneChar));//u：表示按无符号十进制整数输出
    printf("sizeof(CharInt)      = %zu   (char 后补 3 字节)\n", sizeof(CharInt));
    printf("sizeof(IntChar)      = %zu   (尾部再补 3 字节)\n", sizeof(IntChar));
    printf("sizeof(HasFunction)  = %zu   (成员函数不占对象内存)\n", sizeof(HasFunction));
    printf("sizeof(HasVirtual)   = %zu   (只有一个 vptr)\n", sizeof(HasVirtual));
    printf("\n-- 基础类型（%zu 位平台）--\n", sizeof(void *) * 8);
    printf("bool=%zu char=%zu short=%zu int=%zu long=%zu long long=%zu\n",
           sizeof(bool), sizeof(char), sizeof(short), sizeof(int), sizeof(long),
           sizeof(long long));
    printf("float=%zu double=%zu long double=%zu\n",
           sizeof(float), sizeof(double), sizeof(long double));
    printf("int*=%zu void*=%zu 函数指针=%zu\n",
           sizeof(int *), sizeof(void *), sizeof(void (*)()));
    Empty a, b;
    printf("\n两个空类对象：%p 与 %p，相差 %ld 字节\n",
           (void *)&a, (void *)&b, (long)((char *)&b - (char *)&a));
    return 0;
}
