#include "person.h"
#include <iostream>

Person::Person(const std::string &name, int age) : m_name(name), m_age(age) {
}

void Person::Say() const {
    std::cout << "我叫 " << m_name << "，今年 " << m_age << " 岁" << std::endl;
}

void Person::Birthday() {
    ++m_age;
}

std::string Person::Name() const {
    return m_name;
}

int Person::Age() const {
    return m_age;
}