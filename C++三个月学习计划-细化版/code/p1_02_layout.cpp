// 例题 1-2：成员顺序决定内存布局；offsetof 与字节 dump
// 观察点：对齐填充出现在哪里、为什么调整顺序能省内存
#include <cstddef>
#include <cstdio>
#include <string>

struct LooseLayout { // 顺序不好：char → double → int
    char c;
    double d;
    int i;
};

struct TightLayout { // 顺序调整后能省 8 字节
    double d;
    int i;
    char c;
};

struct WithString { // 含库类型：std::string 在 libc++ 里是 24 字节
    int i;
    std::string s;
    char c;
};

template <typename T> static void dump_bytes(const T &obj, const char *name) {
    const unsigned char *p = reinterpret_cast<const unsigned char *>(&obj);
    printf("%-12s (%2zu 字节): ", name, sizeof(T));
    for (size_t i = 0; i < sizeof(T); ++i)
        printf("%02x ", p[i]);
    printf("\n");
}

int main() {
    printf("sizeof(LooseLayout) = %zu\n", sizeof(LooseLayout));
    printf("  offsetof c=%zu d=%zu i=%zu\n", offsetof(LooseLayout, c),
           offsetof(LooseLayout, d), offsetof(LooseLayout, i));
    printf("sizeof(TightLayout) = %zu\n", sizeof(TightLayout));
    printf("  offsetof d=%zu i=%zu c=%zu\n", offsetof(TightLayout, d),
           offsetof(TightLayout, i), offsetof(TightLayout, c));
    printf("同样的三个成员，顺序不同差了 %zu 字节。\n\n",
           sizeof(LooseLayout) - sizeof(TightLayout));

    LooseLayout loose{};
    loose.c = 0xAA;
    loose.d = 1.0;
    loose.i = 0x11223344;
    dump_bytes(loose, "LooseLayout");
    printf("  → 0xAA 后面的 7 个 0 是填充，int 落在偏移 16。\n");

    TightLayout tight{};
    tight.d = 1.0;
    tight.i = 0x11223344;
    tight.c = 0xAA;
    dump_bytes(tight, "TightLayout");
    printf("  → 填充被挤到尾部，只剩 3 字节。\n\n");

    printf("sizeof(std::string) = %zu（libc++ 固定 24 字节）\n",
           sizeof(std::string));
    printf("sizeof(WithString)  = %zu  offsetof(s)=%zu offsetof(c)=%zu\n",
           sizeof(WithString), offsetof(WithString, s),
           offsetof(WithString, c));
    WithString ws{};
    ws.i = 1;
    ws.s = "abc";
    ws.c = 'x';
    dump_bytes(ws, "WithString");
    printf("  → 前 4 字节是 i，3 字节填充，再往后 24 字节是 string 本体。\n");
    return 0;
}
