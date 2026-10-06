#include <cstdio>
#include <iostream>
using namespace std;

int main() {
    std::string name = "Bro";
    int age = 21;
    bool student = true;

    cout << &name << '\n';
    cout << &age << '\n';
    cout << &student << '\n';
    printf("name address is %p, %d \n", &name, &name);
    printf("age address is %p,%d \n", &age, &age);
    printf("student address is %p,%d\n", &student, &student);
    return 0;
}
