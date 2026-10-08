#include "Student.h"
#include <iostream>

// 程序入口。
int main() {
    // 输出定义在 Student.cpp 中的及格分数常量。
    std::cout << "及格线是" << PASS_SCORE << std::endl;

    // 创建姓名为 Tom、分数为 85 的学生。
    Student s("Tom", 85);

    // 常量引用允许读取对象，但不能通过该引用修改对象。
    const Student &ref = s;

    // 输出姓名、分数，并根据分数与及格线的比较结果输出状态。
    std::cout << ref.getName() << "分数" << ref.getScore()
              << (ref.getScore() >= PASS_SCORE ? "及格" : "不及格")
              << std::endl;

    // 返回 0 表示程序正常结束。
    return 0;
}
