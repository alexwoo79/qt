#include <iostream>

int main() {
    // sizeof() with arrays of different types,determines the size in bytes of a:
    // variable, data type, class, objects, etc.

    int x = 5;
    std::cout << "Size of x: " << sizeof(x) << " bytes" << '\n';
    double y = 10.5;
    std::cout << "Size of y: " << sizeof(y) << " bytes" << '\n';
    char z = 'a';
    std::cout << "Size of z: " << sizeof(z) << " bytes" << '\n';

    std::string name = "Alex";
    std::cout << "Size of name: " << sizeof(name) << " bytes" << '\n';

    bool student = true;
    std::cout << "Size of student: " << sizeof(student) << " bytes" << '\n';

    char grades[] = {'A', 'B', 'C'};
    std::cout << "Size of grades: " << sizeof(grades) << " bytes" << '\n';

    std::string fruits[] = {"Apple", "Banana", "Cherry"};
    std::cout << "Size of fruits: " << sizeof(fruits) << " bytes" << '\n';

    // calculate the size of the array in terms of number of elements
    std::cout << "Number of elements in grades: " << sizeof(grades) / sizeof(grades[0]) << '\n';
    std::cout << "Number of elements in fruits: " << sizeof(fruits) / sizeof(std::string) << '\n';

    return 0;
}