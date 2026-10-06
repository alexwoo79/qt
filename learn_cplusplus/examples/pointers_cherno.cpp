#include <iostream>

#define LOG(x) std::cout << x << std::endl;

int main() {
    // int a = 5;
    // int* p = &a;
    // LOG("Value of a: " << a);
    // LOG("Address of a: " << &a);
    // LOG("Value of p (address stored in p): " << p);
    // LOG("Value pointed to by p: " << *p);
    // return 0;
    int var = 8;
    int* ptr = &var;

    // Note: void pointers cannot be directly dereferenced.
    *ptr = 10;
    LOG(var);

    std::cin.get();
}