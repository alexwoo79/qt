// guessing_number.cpp -- a simple number guessing game
#include <iostream>
int main() {
    int num;

    int guess{0};

    int tries{0};

    srand(time(NULL));

    num = (rand() % 100) + 1;

    std::cout << "****** SIMPLE NUMBER GUESSING GAME ******\n";

    while (guess != num) {
        std::cout << "Enter a guess between (1-100): ";
        std::cin >> guess;
        ++tries;

        if (guess > num) {
            std::cout << "Too high!\n";
        } else if (guess < num) {
            std::cout << "Too low!\n";
        } else {
            std::cout << "Congratulations! You guessed the number in " << tries << " tries!\n";
        }
    };

    return 0;
}