#include <iostream>

using namespace std;
// function prototype for searching an array
int searchArray(std::string array[], int size, std::string element);
// main function
int main() {
    std::string foods[] = {"apple", "banana", "cherry",   "date", "elderberry",
                           "fig",   "grape",  "honeydew", "kiwi", "lemon"};
    int size = sizeof(foods) / sizeof(foods[0]);
    int index;
    std::string myFood;

    // 交互
    cout << "Enter a food to search: ";
    std::getline(cin, myFood);

    index = searchArray(foods, size, myFood);

    if (index != -1) {
        cout << "food found at index: " << index << endl;
    } else {
        cout << "food not found." << endl;
    }

    return 0;
}
// function to search for a food in the array
int searchArray(std::string array[], int size, std::string element) {
    for (int i = 0; i < size; ++i) {
        if (array[i] == element) {
            return i;
        }
    }
    return -1; // element not found in the array
}