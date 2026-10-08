#include "Student.h"

// 定义并初始化头文件中声明的外部常量，供其他源文件使用。
extern const int PASS_SCORE = 60;

// 初始化列表为姓名和分数赋初值；成员按头文件中的声明顺序初始化。
Student::Student(const std::string &n, int s) : name(n), score(s) {}

// 返回学生分数，不修改对象。
int Student::getScore() const { return score; }

// 返回学生姓名的副本，不修改对象。
std::string Student::getName() const { return name; }

// 将学生分数更新为传入的值。
void Student::setScore(int s) { score = s; }
