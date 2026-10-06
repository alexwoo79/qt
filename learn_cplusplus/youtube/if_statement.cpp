#include <iostream>

int main() {
    int age;

    std::cout << "Enter your age: ";
    std::cin >> age;

    if (age >= 18 && age <= 100) {
        std::cout << "You are an adult." << std::endl;
    } else if (age < 0){
        std::cout << "Invalid age." << std::endl;
    } else if (age > 100) {
        std::cout << "Invalid old age." << std::endl;
    } else {
        std::cout << "You are a minor." << std::endl;
    }

    return 0;
}