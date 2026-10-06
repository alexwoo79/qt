#include <iostream>
int main() {

    // Demonstrate basic arithmetic operators
    double students =20;
    // Increment the number of students by 1
    // students += 2;
    // students++; // Increment the number of students by 1
    // students = students - 1;
    // students-= 1;
    students--; // Decrement the number of students by 1
    // students *=2;
    students /= 2; // Divide the number of students by 2

    int remainder = static_cast<int>(students) % 2; // Calculate the remainder when dividing the number of students by 2


    std::cout << "Number of students: " << students << std::endl;
    std::cout << "Remainder when divided by 2: " << remainder << std::endl;

    return 0;
}