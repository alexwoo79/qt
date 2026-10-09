// 为 p1_03_stack_heap 的内存结构图取真实数据：Node 的 sizeof/offsetof、栈帧间距、
// 局部变量地址、堆块间距，以及三段地址区间（栈 / 堆 / 静态区）的实际落点。
// 用途：换平台复核 —— 结论是否与平台无关、哪些数值随平台变。
//
//   c++ -std=c++17 -O0 memory-maps/probe_p1_03.cpp -o /tmp/probe_p1_03 && /tmp/probe_p1_03
#include <cstddef>
#include <cstdio>
#include <cstring>

#if defined(__linux__)
#define PROBE_HAS_MAPS 1
#endif

struct Node {
    int value;
    Node *next;
};

static int g_counter = 0;  // .bss：静态存储期
int g_init = 5;            // .data

// 只有一个 int 的帧：每层差多少？
static void frame(int depth, const void *prev) {
    int local = depth;
    printf("frame %d %p delta %lld\n", depth, (const void *)&local,
           prev ? (long long)((const char *)prev - (const char *)&local) : 0LL);
    if (depth < 3)
        frame(depth + 1, &local);
}

// 同样的帧，多一个 char[64]：栈帧大小 = 局部变量之和（对齐后）
static void frame_buf(int depth, const void *prev) {
    char buf[64];
    memset(buf, depth, sizeof buf);
    printf("framebuf %d %p delta %lld\n", depth, (const void *)buf,
           prev ? (long long)((const char *)prev - (const char *)buf) : 0LL);
    if (depth < 3)
        frame_buf(depth + 1, buf);
}

int main() {
    printf("size Node %zu align %zu value %zu next %zu\n", sizeof(Node), alignof(Node),
           offsetof(Node, value), offsetof(Node, next));
    printf("size int %zu pointer %zu\n", sizeof(int), sizeof(void *));

    frame(0, nullptr);
    frame_buf(0, nullptr);

    int a = 1, b = 2, c = 3;
    printf("locals a %p b %p c %p\n", (const void *)&a, (const void *)&b, (const void *)&c);
    int arr[4] = {0, 1, 2, 3};
    printf("array %p step %zu\n", (const void *)&arr[0], sizeof(arr[1]));

    Node *head = nullptr;
    for (int i = 0; i < 3; ++i) {
        Node *node = new Node{i, head};
        printf("heap node%d %p delta %lld\n", i, (const void *)node,
               head ? (long long)((const char *)node - (const char *)head) : 0LL);
        head = node;
    }
    printf("head %p size %zu\n", (const void *)&head, sizeof(head));

    // 一个节点的原始字节：value 在 0..3，next 在 8..15，中间 4 字节是填充（内容未定义）
    Node *sample = new Node{2, head};
    unsigned char raw[sizeof(Node)];
    memcpy(raw, sample, sizeof raw);
    printf("raw node2");
    for (size_t i = 0; i < sizeof raw; ++i)
        printf(" %02x", raw[i]);
    printf("\n");
    delete sample;

    {
        int scoped = 42;
        printf("scoped %p\n", (const void *)&scoped);
    }
    int reused = 7;
    printf("reused %p\n", (const void *)&reused);
    printf("globals g_counter %p g_init %p\n", (const void *)&g_counter, (const void *)&g_init);

#ifdef PROBE_HAS_MAPS
    FILE *maps = fopen("/proc/self/maps", "r");
    char line[512];
    while (maps && fgets(line, sizeof line, maps)) {
        if (strstr(line, "[stack") || strstr(line, "[heap") || strstr(line, "probe_p1_03"))
            printf("maps %s", line);
    }
    if (maps)
        fclose(maps);
#endif

    while (head) {
        Node *next = head->next;
        delete head;
        head = next;
    }
    return 0;
}
