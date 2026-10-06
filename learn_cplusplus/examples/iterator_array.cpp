#include <iostream>

int main() {
    // std::string students[] = {"Alice", "Bob", "Charlie", "Alex"};

    char grades[] = {'A', 'B', 'C', 'D', 'F'};

    for (int i = 0; i < sizeof(grades) / sizeof(char); i++) {
        std::cout << grades[i] << std::endl;
    }
}