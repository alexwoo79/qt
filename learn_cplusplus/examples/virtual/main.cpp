#include <cstddef>
#include <cstdio>
#include <iostream>
#include <string>
using namespace std;

// 空类仍然必须能创建彼此独立的对象，所以 sizeof(Empty) 至少为 1。
// 具体大小由实现决定；常见编译器上通常是 1 字节。
class Empty {};

// 普通成员类：对象中包含一个 int 和一个 std::string 对象。
// sizeof(PlainStudent) 只计算对象本身，不计算 string 另外申请的堆内存。
class PlainStudent {
  private:
    int score;
    string name;

  public:
    PlainStudent(const string &n, int s) : score(s), name(n) {}
};

// 多态类：有虚函数，因此可以通过基类指针/引用进行动态派发。
// 常见实现会在对象中放一个隐藏的虚表指针（vptr），但 C++ 标准不规定
// 必须使用 vptr，也不规定它的位置、大小或虚表的具体布局。
class VirtualStudent {
  private:
    int score;
    string name;

  public:
    VirtualStudent(const string &n, int s) : score(s), name(n) {}
    virtual int getScore() const { return score; }
};

int main() {
    // sizeof 给出对象类型的大小，单位是字节；它不是“成员大小简单相加”。
    // 结果可能包含对齐填充，也会包含实现为支持多态而放入对象的隐藏数据。
    cout << "空类大小：" << sizeof(Empty) << "字节\n";
    cout << "普通成员类大小：" << sizeof(PlainStudent) << "字节\n";
    cout << "带虚函数类大小：" << sizeof(VirtualStudent) << "字节\n\n";

    // 创建一个实际对象，然后按字节查看它的对象表示。
    VirtualStudent stu("Tom", 90);

    // 通过 unsigned char 查看对象表示中的字节是允许的。
    // 但这不是可移植的“类布局说明”：成员的实际排布、填充字节、
    // std::string 的内部表示、字节序以及虚表实现都可能因平台而不同。
    unsigned char *p = reinterpret_cast<unsigned char *>(&stu);
    cout << "VirtualStudent对象内存字节（偏移: 值）：\n";
    for (size_t i = 0; i < sizeof(VirtualStudent); i++) {
        printf("%02zu: %02X\n", i, p[i]);
    }

    // 90 的十六进制确实是 0x5A，但转储中未必能直接找到一个独立的 5A：
    // int 的字节顺序取决于字节序，成员之间可能有填充，其他数据也可能含 5A。
    // 常见实现会有 vptr，但不能仅凭某几个字节断定它的位置或长度。
    cout << "\n提示：90 的十六进制是 0x5A；上面的字节只展示当前实现的对象表示，"
            "不要把某段字节当作跨平台固定布局。\n";
    return 0;
}
