#include <iostream>

using namespace std;
// Function prototype for sorting an array in ascending order
void sort(int array[], int size);
// main function
int main() {
    int array[] = {10, 1, 5, 3, 7, 2, 8, 6, 4, 9};
    int size = sizeof(array) / sizeof(array[0]);
    // Display the original array before sorting
    cout << "Original array: " << endl;
    for (int i = 0; i < size; ++i) {
        cout << array[i] << " ";
    }
    cout << endl;
    // Sort the array in ascending order
    sort(array, size);

    // Display the sorted array after sorting
    cout << "Sorted array: " << endl;
    for (int i = 0; i < size; ++i) {
        cout << array[i] << " ";
    }
    cout << endl;

    return 0;
}
// Function to sort an array in ascending order
void sort(int array[], int size) {
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - i - 1; ++j) {
            if (array[j] > array[j + 1]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}