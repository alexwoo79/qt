#include <iostream>
#include <vector>

// typedef std::string text_t;// Alias for std::string 
// typedef int number_t; // Alias for int

using text_t = std::string; // Alias for std::string
using number_t = int; // Alias for int
int main() {
    text_t firstName = "Alex";
    text_t lastName = "Smith";
    std::cout << "Full name: " << firstName << " " << lastName << std::endl;
    number_t age = 30;
    std::cout << "Age: " << age << std::endl;
    return 0;
}