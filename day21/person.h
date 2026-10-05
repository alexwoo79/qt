// person.h
#ifndef PERSON_H
#define PERSON_H
#include <string>

// 类声明：告诉编译器"有这样一个类，它有什么"
class Person {
  public:
    // 构造函数声明
    Person(const std::string &name, int age);

    // 方法声明（注意：末尾有分号，没有函数体）
    void Say() const;
    void Birthday();

    // 访问器声明
    std::string Name() const;
    int Age() const;

  private:
    // 成员变量声明
    std::string m_name;
    int m_age;
};

#endif // PERSON_H
