#include <cctype>
#include <iostream>
#include <random>

// func declarations
char getUserChoice();
char getComputerChoice();
void showChoice(char choice);
void chooseWinner(char player, char computer);

// main function
int main() {
    char playerChoice = getUserChoice();
    if (playerChoice == '\0') {
        return 0;
    }

    char computerChoice = getComputerChoice();
    std::cout << "You chose: ";
    showChoice(playerChoice);
    std::cout << "Computer chose: ";
    showChoice(computerChoice);
    chooseWinner(playerChoice, computerChoice);
    return 0;
}

// function definitions
char getUserChoice() {
    char player;
    std::cout << "Rock-Paper-Scissors Game!\n";
    // do - while loop to get a valid user choice
    do {
        std::cout << "Make your choice:\n";
        std::cout << "'r' for rock\n";
        std::cout << "'p' for paper\n";
        std::cout << "'s' for scissors\n";
        std::cout << "Enter your choice: ";

        // if no input is provided, return null character
        if (!(std::cin >> player)) {
            return '\0';
        }
        // convert the input to lowercase for consistency
        player = static_cast<char>(std::tolower(static_cast<unsigned char>(player)));
        if (player != 'r' && player != 'p' && player != 's') {
            std::cout << "Invalid choice. Please enter 'r', 'p', or 's'.\n";
        }
    } while (player != 'r' && player != 'p' && player != 's');
    return player;
}

char getComputerChoice() {
    // mt19937 random number generator for the computer's choice
    static std::mt19937 generator(std::random_device{}());
    // uniform distribution to select between rock, paper, and scissors
    static std::uniform_int_distribution<int> distribution(0, 2);
    const char choices[] = {'r', 'p', 's'};
    return choices[distribution(generator)];
}

void showChoice(char choice) {
    switch (choice) {
    case 'r':
        std::cout << "Rock\n";
        break;
    case 'p':
        std::cout << "Paper\n";
        break;
    case 's':
        std::cout << "Scissors\n";
        break;
    }
}

void chooseWinner(char player, char computer) {
    if (player == computer) {
        std::cout << "It's a tie!\n";
    } else if ((player == 'r' && computer == 's') || (player == 'p' && computer == 'r') ||
               (player == 's' && computer == 'p')) {
        std::cout << "You win!\n";
    } else {
        std::cout << "You lose!\n";
    }
}