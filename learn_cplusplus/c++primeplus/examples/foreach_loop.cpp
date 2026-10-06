#include <iostream>

using namespace std;

int main() {
    // std::string students[] = {"Alice", "Bob", "Charlie", "Alex"};

    // Using a range-based for loop to iterate over the array of students
    // for (const auto &student : students) {
    //     std::cout << student << std::endl;
    // // }
    // for (std::string student : students) {
    //     std::cout << student << std::endl;
    // }
    int grades[] = {90, 85, 78, 92, 88};

    // for (int grade : grades) {
    //     std::cout << grade << std::endl;
    // }
    for (const auto &grade : grades) {
        std::cout << grade << std::endl;
    }

    return 0;
}
