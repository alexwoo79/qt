#include <iostream>

int main() {
    //cout << (insert variable here) << std::endl;
    //cin >> variable;
    std::string name;

    int age;
    std::cout << "Enter your age: ";
    std::cin >> age;
    
    std::cout << "Enter your name: ";
    std::getline(std::cin >> std::ws, name); 
    //会先从输入流中移除开头的空白字符，包括空格、制表符和换行符。

    std::cout << "Hello, " << name << "!" << std::endl;
    std::cout << "You are " << age << " years old." << std::endl;
    return 0;
}