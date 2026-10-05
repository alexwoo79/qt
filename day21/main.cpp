// main.cpp
#include "person.h" // 只需要包含头文件
#include <iostream>

int main() {
    // 创建一个 Person 对象（调用构造函数）
    Person p("张三", 25);

    // 调用方法
    p.Say();

    // 过生日
    p.Birthday();
    p.Say();

    // 用访问器读值
    std::cout << "姓名: " << p.Name() << std::endl;
    std::cout << "年龄: " << p.Age() << std::endl;

    return 0;
}