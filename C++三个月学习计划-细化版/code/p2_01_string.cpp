// 例题 2-1：std::string 的 SSO、容量与扩容
// 观察点：小字符串的缓冲就在对象内部；越界那一刻数据会搬到堆上
#include <cstdint>
#include <cstdio>
#include <string>

static void show(const std::string &s, const char *label) {
    const char *data = s.data();
    const char *self = reinterpret_cast<const char *>(&s);
    bool inside = data >= self && data < self + sizeof(s);
    printf("%-10s size=%2zu capacity=%2zu data=%p SSO=%s\n",
           label, s.size(), s.capacity(), (const void *)data, inside ? "内部缓冲" : "堆上");
}

int main() {
    printf("sizeof(std::string) = %zu（libc++：指针/长度/容量 24 字节，SSO 缓冲嵌在里面）\n\n",
           sizeof(std::string));

    std::string shortStr = "hi";
    std::string mediumStr = "01234567890123456789";      // 20 字符
    std::string longStr = "0123456789012345678901234567890";  // 31 字符
    show(shortStr, "短字符串");
    show(mediumStr, "20 字符");
    show(longStr, "31 字符");
    printf("→ libc++ 的 SSO 上限是 22 个字符：不超过就放在对象内部，不碰堆。\n\n");

    printf("-- 逐步增长，看容量怎么跳 --\n");
    std::string s;
    for (int i = 0; i < 40; ++i) {
        s += 'x';
        if (i < 4 || i == 20 || i == 21 || i == 22 || i == 23 || i == 39) {
            printf("长度 %2d：capacity=%2zu data=%p%s\n", (int)s.size(), s.capacity(),
                   (const void *)s.data(),
                   s.capacity() > 22 ? "  ← 已经在堆上了" : "");
        }
    }
    printf("\n-- 扩容会重新分配 + 拷贝，预留容量可避免 --\n");
    std::string r;
    r.reserve(64);
    const char *before = r.data();
    for (int i = 0; i < 40; ++i) r += 'y';
    printf("reserve(64) 后：capacity=%zu 追加 40 次，data 地址%s\n",
           r.capacity(), r.data() == before ? "没变（一次分配搞定）" : "变了（发生了重分配）");
    return 0;
}
