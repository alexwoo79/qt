#include <iostream>

using namespace std;
// function prototype for searching an array
int searchArray(int array[], int size, int element);
// main function
int main() {
    int numbers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int size = sizeof(numbers) / sizeof(numbers[0]);
    int index;
    int myNumber;

    // 交互
    cout << "Enter a number to search: ";
    cin >> myNumber;

    index = searchArray(numbers, size, myNumber);

    if (index != -1) {
        cout << "Number found at index: " << index << endl;
    } else {
        cout << "Number not found." << endl;
    }

    return 0;
}
// function to search for a number in the array
int searchArray(int array[], int size, int element) {
    for (int i = 0; i < size; ++i) {
        if (array[i] == element) {
            return i;
        }
    }
    return -1; // element not found in the array
}