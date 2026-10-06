#include <iostream>

int main() {

    std::string car[3];

    car[0] = "Camero";
    car[1] = "Mustang";
    car[2] = "Charger";
    std::cout << car[0] << '\n';
    std::cout << car[1] << '\n';
    std::cout << car[2] << '\n';

    double prices[] = {19.99, 29.99, 39.99};
    std::cout << prices[0] << '\n';
    std::cout << prices[1] << '\n';
    std::cout << prices[2] << '\n';
    return 0;
}