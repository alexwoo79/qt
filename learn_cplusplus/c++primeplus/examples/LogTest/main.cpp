#include "include/log.cpp"
#include <iostream>
#include <string>

int main() {
    Log("Hello from main.cpp!");
    Log("Goodbye from main.cpp!");

    Log("Now, please enter something:");
    std::string input;
    std::getline(std::cin, input);
    Log("You entered: " + input);
    return 0;
}