#include <iostream>

int generate_random_number(int max_value) {
    return rand() % max_value + 1;
}

int main() {
    int num1, num2, num3, max_value = 6;
    srand(time(NULL));                        // Seed the random number generator with the current time
    num1 = generate_random_number(max_value); // Generate a random number between 1 and max_value
    num2 = generate_random_number(max_value);
    num3 = generate_random_number(max_value);
    std::cout << "Random number 1: " << num1 << std::endl;
    std::cout << "Random number 2: " << num2 << std::endl;
    std::cout << "Random number 3: " << num3 << std::endl;
    return 0;
}