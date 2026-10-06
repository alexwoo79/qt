#include <iostream>

using namespace std;

int getDigits(const int number);
int sumEvenDigits(const string &cardNumber);
int sumOddDigits(const string &cardNumber);

int main() {
    string cardNumber;
    int result = 0;

    cout << "Enter a credit card #: ";
    cin >> cardNumber;

    // Luhn check: from the right, double every second digit starting at the second-to-last.
    // Sum the digits of doubled values, add the untouched digits, and check divisibility by 10.
    result = sumEvenDigits(cardNumber) + sumOddDigits(cardNumber);

    if (result % 10 == 0) {
        cout << "Credit card number is valid." << endl;
    } else {
        cout << "Credit card number is invalid." << endl;
    }

    return 0;
}

int getDigits(const int number) {
    return number % 10 + (number / 10 % 10);
}
int sumEvenDigits(const string &cardNumber) {
    int sum = 0;
    for (int i = cardNumber.length() - 2; i >= 0; i -= 2) {
        sum += getDigits((cardNumber[i] - '0') * 2);
    }
    return sum;
}

int sumOddDigits(const string &cardNumber) {
    int sum = 0;
    for (int i = cardNumber.length() - 1; i >= 0; i -= 2) {
        sum += cardNumber[i] - '0';
    }
    return sum;
}
