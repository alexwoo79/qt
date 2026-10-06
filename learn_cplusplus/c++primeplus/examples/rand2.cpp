#include <ctime>
#include <iostream>
int main() {
    srand(time(0));
    int randNum = rand() % 5 + 1;
    switch (randNum) {
    case 1:
        std::cout << "You win a bumper sticker!\n";
        break;
    case 2:
        std::cout << "You win a T-shirt!\n";
        break;
    case 3:
        std::cout << "You win a pen!\n";
        break;
    case 4:
        std::cout << "You win a cat!\n";
        break;
    case 5:
        std::cout << "You win a dog!\n";
        break;
    case 6:
        std::cout << "You win a hamster!\n";
        break;
        return 0;
    }
}