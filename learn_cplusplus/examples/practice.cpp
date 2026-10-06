#include <iostream>
#include <cmath>

int main() {
    //triangle, using Pythagorean theorem to calculate the length of side c
    double a;
    double b;
    double c;
    std::cout << "Enter the length of side a: ";
    std::cin >> a;
    std::cout << "Enter the length of side b: ";
    std::cin >> b;
    

    a = pow(a,2);
    b = pow(b,2);
    c = sqrt(a + b);
    std::cout << "The length  of c is: " << c << std::endl;
}