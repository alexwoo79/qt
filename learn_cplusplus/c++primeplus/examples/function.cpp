#include <iostream>

void happyBirthday(std::string name, int age);

int main() {
    happyBirthday("Alex", 25);
    return 0;
}

void happyBirthday(std::string name, int age) {
    std::cout << "Happy Birthday, " << name << "! You are now " << age << " years old.\n";
}