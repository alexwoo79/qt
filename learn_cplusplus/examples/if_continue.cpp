#include <iostream>

int main() {
    for (int i = 0; i < 10; ++i) {
        if ((i+1) % 2 == 0) {
            // continue;// Skip even numbers
            break; // Exit the loop when an even number is encountered
        }
        std::cout << i << std::endl;
    }
    return 0;
}