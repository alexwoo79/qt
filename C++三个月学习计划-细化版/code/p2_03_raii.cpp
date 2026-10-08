// 例题 2-3：RAII —— 把"释放"这件事交给析构函数
// 观察点：无论正常返回、提前 return 还是抛异常，析构都会执行，账本一定归零
#include <cstddef>
#include <cstdio>
#include <stdexcept>
#include <utility>
#include <vector>

static int g_outstanding = 0;  // 当前还没释放的堆分配数（内存账本）

class Buffer {
  public:
    Buffer(std::size_t n, const char *tag)
        : tag_(tag), size_(n), data_(new unsigned char[n]) {
        ++g_outstanding;
        printf("  构造 %-8s：申请 %zu 字节 @ %p（账本=%d）\n", tag_, n, (void *)data_, g_outstanding);
    }
    Buffer(const Buffer &) = delete;             // 独占资源，禁止拷贝
    Buffer &operator=(const Buffer &) = delete;
    Buffer(Buffer &&other) noexcept              // 只允许移动：转移所有权
        : tag_(other.tag_), size_(other.size_), data_(other.data_) {
        other.data_ = nullptr;
        other.size_ = 0;
        printf("  移动 %-8s：指针移交给新对象（账本不变=%d）\n", tag_, g_outstanding);
    }
    ~Buffer() {
        if (data_) {
            delete[] data_;
            --g_outstanding;
            printf("  析构 %-8s：释放 @ %p（账本=%d）\n", tag_, (void *)data_, g_outstanding);
        }
    }
    const char *tag() const { return tag_; }
    std::size_t size() const { return size_; }

  private:
    const char *tag_;
    std::size_t size_;
    unsigned char *data_;
};

static void raiiVersion() {
    printf("[RAII 版本]\n");
    Buffer first(1024, "first");
    Buffer second(2048, "second");
    printf("  → 出作用域时按构造的逆序析构：\n");
}

static void rawVersion() {
    printf("[裸指针版本]\n");
    unsigned char *raw = new unsigned char[1024];
    ++g_outstanding;
    printf("  new 了 1024 字节 @ %p，但忘了 delete\n", (void *)raw);
}

static void throwVersion() {
    printf("[中途抛异常]\n");
    Buffer guard(512, "guard");
    throw std::runtime_error("模拟失败");
}

int main() {
    raiiVersion();
    printf("  账本 = %d\n\n", g_outstanding);

    rawVersion();
    printf("  账本 = %d  ← 这 1 就是泄漏，ASan/LSan 会报 leak\n\n", g_outstanding);
    g_outstanding = 0;  // 演示用：人为清账，避免影响后面的输出

    try {
        throwVersion();
    } catch (const std::exception &e) {
        printf("  捕获异常：%s\n\n", e.what());
    }
    printf("  账本 = %d（异常路径也被清理干净）\n\n", g_outstanding);

    printf("[把 Buffer 放进 vector：只移动，不拷贝]\n");
    {
        std::vector<Buffer> pool;
        pool.reserve(2);
        pool.emplace_back(128, "pool-1");
        pool.emplace_back(256, "pool-2");
        pool.emplace_back(512, "pool-3");  // 触发扩容：已有元素被移动
        printf("  vector 内元素数=%zu，账本=%d\n", pool.size(), g_outstanding);
    }
    printf("  vector 销毁后账本 = %d\n", g_outstanding);
    return 0;
}
