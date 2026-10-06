#include <iostream> // 提供 std::cout、std::endl 和 std::cin。

#define LOG(x) std::cout << x << std::endl; // 将参数输出到控制台并换行。

struct Entity {
    static int x; // 声明静态成员：它属于 Entity 类型，而不是某个对象。
    static int y; // 同上；所有 Entity 对象访问的都是同一个 y。
};

int Entity::x = 0; // 在类外定义 x 并提供存储空间；整个程序只需要一份定义。
int Entity::y = 0; // 在类外定义 y 并提供存储空间；初始化值为 0。

int main() {
    Entity e1; // 创建第一个对象；它不会拥有独立的 x 和 y。

    Entity::x = 2; // 通过类名修改共享的静态成员 x。
    Entity::y = 3; // 通过类名修改共享的静态成员 y。

    LOG(Entity::x << ", " << Entity::y) // 输出当前共享值：2, 3。
    Entity e2;     // 再创建一个对象；它仍然访问同一份 x 和 y。
    Entity::x = 5; // 修改共享的 x，之前创建的 e1 看到的也是这个值。
    Entity::y = 8; // 修改共享的 y，所有 Entity 对象看到的都是这个值。

    LOG(Entity::x << ", " << Entity::y) // 输出更新后的共享值：5, 8。

    std::cin.get(); // 等待用户输入一个字符后继续，便于在终端查看输出。
    return 0;       // 返回 0 表示程序正常结束。
}

/*
更新了待办事项列表

`static`
的含义取决于它出现的位置，核心可以理解为：**改变变量的生命周期、可见范围，或成员与对象的关系**。

| 位置 | 影响 |
|---|---|
| 函数内的变量 | 只初始化一次，函数返回后仍保留值；再次调用时继续使用原值。 |
| 命名空间/文件作用域的变量 | 具有静态存储期，并且通常只在当前源文件可见。 |
| 类中的数据成员 | 属于类本身，由该类的所有对象共享，不会为每个对象各存一份。 |
| 类中的成员函数 | 不依赖某个对象，没有 `this` 指针；不能直接访问非静态成员。 |

例如类的静态成员：

```cpp
struct Entity {
    static int count; // 声明：Entity 的所有对象共享 count
};

int Entity::count = 0; // 定义：提供存储空间，通常需要在类外定义

int main() {
    Entity first;
    Entity second;

    Entity::count = 2; // 通过类名访问
    first.count = 3;   // 通过对象访问也可以，修改的是同一个 count
    // 此时 second.count 也是 3
}
```

因此，你之前示例中的 `static int x;` 说明 `x`
是共享的类成员，但这行本身只是声明；`int Entity::x = 0;`
才为它提供定义和存储空间。C++17 起也可以写成 `inline static int x =
0;`，直接在类内定义。

**C++ 没有 `static class`
这种类声明语法。**通常通过把类的构造函数设为私有、只提供静态成员等方式，限制类的实例化。
*/