#include <iostream>

using namespace std;

int main() {
    int *pointer = nullptr; // pointer that is not pointing to any memory address

    int x = 123;

    pointer = &x;
    cout << "Pointer is now pointing to x: " << pointer << endl;
    if (pointer == nullptr) {
        cout << "Pointer is null" << endl;

    } else {
        cout << "Pointer is not null" << endl;
        cout << "Dereferenced pointer: " << *pointer << endl;
    }
    return 0;
}
