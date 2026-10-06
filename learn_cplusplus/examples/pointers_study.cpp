#include <iostream>

using namespace std;

int main() {
    // pointers = variable that stores a memory address of another variable
    // & address-of operator
    // * dereference operator

    std::string name = "Alex";
    int age = 25;
    std::string freePizzas[5] = {"Margherita", "Pepperoni", "Hawaiian", "Veggie", "BBQ Chicken"};

    std::string *pName = &name;            // pointer to the memory address of name
    int *pAge = &age;                      // pointer to the memory address of age
    std::string *pFreePizzas = freePizzas; // pointer to the memory address of the first element of the array
    // array be decayed to pointer when assigned to pFreePizzas, meaning pFreePizzas points to the first element of the

    cout << "Name: " << name << endl;
    cout << "Pointer to name: " << pName << endl;
    cout << "Dereferenced pointer: " << *pName << endl;

    cout << "Age: " << age << endl;
    cout << "Pointer to age: " << pAge << endl;
    cout << "Dereferenced pointer: " << *pAge << endl;

    cout << "Free pizzas: " << endl;
    for (int i = 0; i < 5; i++) {
        cout << "Pointer to freePizzas[" << i << "]: " << (pFreePizzas + i) << endl;
        cout << "Dereferenced pointer: " << *(pFreePizzas + i) << endl;
    }
    return 0;
}
