#include <cmath>
#include <iostream>

int main() {
    double a = 1, b = 2, c = 3, d = 4, e, f = 5;
    e = std::log(sinf(atan2f(hypot(abs(a + b), abs(c)), d + f)));
    std::cout << "e = " << e << '\n';
}