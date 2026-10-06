#include <iostream>

using namespace std;

int main() {

    std::string questions[] = {"1. What year was C++ created?: ", "2. Who invented C++?: ",
                               "3. What is the predecessor of C++?: ", "4, is the Earth flat?"};

    std::string options[][4] = {
        {"A.1983", "B.1985", "C.1987", "D.1990"},
        {"A. Bjarne Stroustrup", "B. James Gosling", "C. Guido van Rossum", "D. Dennis Ritchie"},
        {"A. C", "B. Java", "C. Python", "D. Pascal"},
        {"A. Yes", "B. No", "C. Maybe", "D. I don't know"}};
    char answerKey[] = {'B', 'A', 'A', 'B'};
    int size = sizeof(questions) / sizeof(questions[0]);
    char guess;
    int score = 0;
    for (int i = 0; i < size; i++) {
        cout << "**************************************\n";
        cout << questions[i] << '\n';
        cout << "**************************************\n";
        for (int j = 0; j < 4; j++) {
            cout << options[i][j] << '\n';
        }
        cout << "Your answer: ";
        cin >> guess;
        guess = toupper(guess);
        if (guess == answerKey[i]) {
            cout << "Correct!\n";
            score++;
        } else {
            cout << "Incorrect. The correct answer is " << answerKey[i] << ".\n";
        }
    }
    cout << "**************************************\n";
    cout << "              RESULTS                 \n";
    cout << "**************************************\n";
    cout << "CORRECT GUESSES: " << score << '\n';
    cout << "# of QUESTIONS: " << size << '\n';
    cout << "SCORE: " << (score / size) * 100 << "%";

    return 0;
}
