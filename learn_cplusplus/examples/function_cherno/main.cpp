#include "Log.cpp"
#include <iostream>

int multiply(int a, int b) {
    return a * b;
}

void multiplyAndLog(int a, int b) {
    int result = multiply(a, b);
    std::cout << a << " * " << b << " = " << result << std::endl;
}

int main() {
    multiplyAndLog(3, 4);
    multiplyAndLog(5, 6);
    multiplyAndLog(7, 8);
    Log("Finished multiplying numbers");
    return 0;
}