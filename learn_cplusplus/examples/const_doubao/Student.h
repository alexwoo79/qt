#pragma once
#include <string>

// 声明在 Student.cpp 中定义的及格分数常量。
extern const int PASS_SCORE;

// 保存学生姓名和分数，并提供读取与修改分数的接口。
class Student {
  private:
    int score;        // 学生分数。
    std::string name; // 学生姓名。

  public:
    // 使用姓名和分数初始化学生对象。
    Student(const std::string &n, int s);

    // const 表示读取操作不会修改当前对象。
    int getScore() const;
    std::string getName() const;

    // 更新学生分数。
    void setScore(int s);
};
