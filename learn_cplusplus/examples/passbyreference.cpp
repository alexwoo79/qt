#include <iostream>

void swap(std::string &x, std::string &y);

int main() {
    std::string x = "Kool-Aid";
    std::string y = "Water";

    std::cout << "Before swap:" << '\n';
    std::cout << "X: " << &x << " value: " << x << '\n';
    std::cout << "Y: " << &y << " value: " << y << '\n';

    swap(x, y);

    std::cout << "After swap:" << '\n';
    std::cout << "X: " << &x << " value: " << x << '\n';
    std::cout << "Y: " << &y << " value: " << y << '\n';

    return 0;
}

void swap(std::string &x, std::string &y) {
    std::string temp;
    temp = x;
    x = y;
    y = temp;
}