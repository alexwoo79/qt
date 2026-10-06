#include "log.h"
#include <iostream>
#include <string>
void Log(const std::string &message) {
    std::cout << message << std::endl;
    // std::cin.get(); // Wait for the user to press Enter before continuing
}
