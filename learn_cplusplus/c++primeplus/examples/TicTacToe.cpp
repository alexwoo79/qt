#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
using namespace std;

void drawBoard(char *space);
bool playerMove(char *space, char player);
void computerMove(char *space, char computer);
bool checkWinner(char *space, char player, char computer);
bool checkTie(char *space);
bool hasWon(const char *space, char marker);

int main() {
    char spaces[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    char player = 'X';
    char computer = 'O';

    srand(static_cast<unsigned int>(time(nullptr)));

    drawBoard(spaces);

    while (true) {
        if (!playerMove(spaces, player)) {
            cout << "\nInput ended. Game over." << endl;
            break;
        }
        drawBoard(spaces);
        if (checkWinner(spaces, player, computer)) {
            break;
        }
        if (checkTie(spaces)) {
            cout << "It's a tie!" << endl;
            break;
        }

        computerMove(spaces, computer);
        drawBoard(spaces);
        if (checkWinner(spaces, player, computer)) {
            break;
        }
        if (checkTie(spaces)) {
            cout << "It's a tie!" << endl;
            break;
        }
    }

    return 0;
}

void drawBoard(char *spaces) {
    cout << '\n';
    cout << "     |     |     " << '\n';
    cout << "  " << spaces[0] << "  |  " << spaces[1] << "  |  " << spaces[2] << "  " << '\n';
    cout << "_____|_____|_____" << '\n';
    cout << "     |     |     " << '\n';
    cout << "  " << spaces[3] << "  |  " << spaces[4] << "  |  " << spaces[5] << "  " << '\n';
    cout << "_____|_____|_____" << '\n';
    cout << "     |     |     " << '\n';
    cout << "  " << spaces[6] << "  |  " << spaces[7] << "  |  " << spaces[8] << "  " << '\n';
    cout << "     |     |     " << '\n';
}

bool playerMove(char *spaces, char player) {
    int number;
    while (true) {
        cout << "Enter a spot to place a marker (1-9): ";
        if (!(cin >> number)) {
            if (cin.eof()) {
                return false;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a number from 1 to 9." << endl;
            continue;
        }

        if (number < 1 || number > 9) {
            cout << "That spot is out of range. Choose 1 through 9." << endl;
            continue;
        }

        --number;
        if (spaces[number] != ' ') {
            cout << "That spot is already taken. Choose an empty spot." << endl;
            continue;
        }

        spaces[number] = player;
        return true;
    }
}

void computerMove(char *spaces, char computer) {
    int emptySpots[9];
    int emptyCount = 0;
    for (int i = 0; i < 9; ++i) {
        if (spaces[i] == ' ') {
            emptySpots[emptyCount++] = i;
        }
    }

    if (emptyCount > 0) {
        int choice = emptySpots[rand() % emptyCount];
        spaces[choice] = computer;
    }
}

bool checkWinner(char *spaces, char player, char computer) {
    if (hasWon(spaces, player)) {
        cout << "Player wins!" << endl;
        return true;
    }
    if (hasWon(spaces, computer)) {
        cout << "Computer wins!" << endl;
        return true;
    }
    return false;
}

bool hasWon(const char *spaces, char marker) {
    const int winningLines[8][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6},
                                    {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};

    for (const auto &line : winningLines) {
        if (spaces[line[0]] == marker && spaces[line[1]] == marker && spaces[line[2]] == marker) {
            return true;
        }
    }
    return false;
}

bool checkTie(char *spaces) {
    for (int i = 0; i < 9; ++i) {
        if (spaces[i] == ' ') {
            return false;
        }
    }
    return true;
}