// 例题 1-3：栈与堆的本质区别（地址方向、生命周期、谁负责释放）
#include <cstdio>

static int g_counter = 0;

static void level(int depth) {
    int local = depth;
    printf("level(%d): 栈变量 &local = %p\n", depth, (void *)&local);
    if (depth < 3) level(depth + 1);
}

struct Node {
    int value;
    Node *next;
};

int main() {
    printf("-- 栈：向低地址方向增长 --\n");
    level(0);
    printf("g_counter（全局/静态区）地址 = %p\n\n", (void *)&g_counter);

    printf("-- 同一函数里的局部变量 --\n");
    int a = 1, b = 2, c = 3;
    printf("&a=%p &b=%p &c=%p（地址不保证连续）\n", (void *)&a, (void *)&b, (void *)&c);
    int arr[4] = {0, 1, 2, 3};
    for (int i = 0; i < 4; ++i) {
        printf("arr[%d] 值=%d 地址=%p（同一数组内连续，步长 %zu）\n",
               i, arr[i], (void *)&arr[i], sizeof(int));
    }

    printf("\n-- 堆：地址由分配器决定，通常与栈相距很远 --\n");
    Node *head = nullptr;
    for (int i = 0; i < 3; ++i) {
        Node *node = new Node{i, head};
        printf("new Node{value=%d} → 堆地址 %p\n", i, (void *)node);
        head = node;
    }
    printf("头指针 head 本身在栈上：&head=%p（%zu 字节），指向堆上的节点\n",
           (void *)&head, sizeof(head));

    printf("\n-- 生命周期 --\n");
    {
        int scoped = 42;
        printf("作用域内的 scoped 在 %p\n", (void *)&scoped);
    }
    int reused = 7;
    printf("作用域外的 reused 在 %p（地址常被复用，说明栈空间是重复利用的）\n",
           (void *)&reused);

    printf("\n-- 释放：栈自动，堆手动 --\n");
    while (head) {
        Node *next = head->next;
        printf("delete 堆节点 %p（值 %d）\n", (void *)head, head->value);
        delete head;
        head = next;
    }
    return 0;
}
