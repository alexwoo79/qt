// 为内存结构图取真实数据：sizeof、成员偏移、对齐
#include <cstddef>
#include <cstdio>
#include <string>

struct Empty {};
struct OneChar { char c; };
struct CharInt { char c; int i; };
struct IntChar { int i; char c; };
struct HasFunction { void f(); };
struct HasVirtual { virtual void f(); };
struct WithVirtual {
    int value;
    virtual void f() {}
    virtual void g(int) {}
};
struct LooseLayout { char c; double d; int i; };
struct TightLayout { double d; int i; char c; };

int main() {
    printf("size Empty %zu\n", sizeof(Empty));
    printf("size OneChar %zu\n", sizeof(OneChar));
    printf("size CharInt %zu c %zu i %zu\n", sizeof(CharInt), offsetof(CharInt, c), offsetof(CharInt, i));
    printf("size IntChar %zu i %zu c %zu\n", sizeof(IntChar), offsetof(IntChar, i), offsetof(IntChar, c));
    printf("size HasFunction %zu\n", sizeof(HasFunction));
    printf("size HasVirtual %zu\n", sizeof(HasVirtual));
    printf("size WithVirtual %zu value %zu\n", sizeof(WithVirtual), offsetof(WithVirtual, value));
    printf("size LooseLayout %zu c %zu d %zu i %zu\n", sizeof(LooseLayout),
           offsetof(LooseLayout, c), offsetof(LooseLayout, d), offsetof(LooseLayout, i));
    printf("size TightLayout %zu d %zu i %zu c %zu\n", sizeof(TightLayout),
           offsetof(TightLayout, d), offsetof(TightLayout, i), offsetof(TightLayout, c));
    printf("align int %zu double %zu void* %zu\n", alignof(int), alignof(double), alignof(void *));
    return 0;
}
