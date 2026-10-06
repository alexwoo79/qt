#include <iomanip>
#include <iostream>
#include <limits>

int choice;
double balance = 0.0;

void showBalance(double balance) {
    std::cout << "Your current balance is: $" << std::setprecision(2) << std::fixed << balance << "\n";
}
void depositMoney(double &balance, double amount) {
    balance += amount;
    std::cout << "Deposited $" << amount << ". New balance is $" << balance << "\n";
}

void withdrawMoney(double &balance, double amount) {
    if (amount > balance) {
        std::cout << "Insufficient funds. Your current balance is $" << balance << "\n";
    } else {
        balance -= amount;
        std::cout << "Withdrew $" << amount << ". New balance is $" << balance << "\n";
    }
}
int main() {
    do {
        std::cout << "******************\n";
        std::cout << "Enter your choice:\n";
        std::cout << "******************\n";
        std::cout << "1. Show Balance\n";
        std::cout << "2. Deposit Money\n";
        std::cout << "3. Withdraw Money\n";
        std::cout << "4. Exit\n";
        if (!(std::cin >> choice)) {
            if (std::cin.eof()) {
                return 0;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid choice. Please try again.\n";
            continue;
        }
        switch (choice) {
        case 1:
            showBalance(balance);
            break;
        case 2:
            double amount;
            std::cout << "Enter amount to deposit: ";
            std::cin >> amount;
            if (std::cin.fail()) {
                std::cout << "Invalid input. Please enter a numeric value.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else if (amount <= 0) {
                std::cout << "Invalid amount. Please enter a positive value.\n";
            } else {
                depositMoney(balance, amount);
            }
            break;

        case 3:
            std::cout << "Enter amount to withdraw: ";
            std::cin >> amount;
            if (std::cin.fail()) {
                std::cout << "Invalid input. Please enter a numeric value.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else if (amount <= 0) {
                std::cout << "Invalid amount. Please enter a positive value.\n";
            } else {
                withdrawMoney(balance, amount);
            }
            break;

        case 4:
            std::cout << "Thanks for visiting!\n";
            return 0;

        default:
            std::cout << "Invalid choice. Please try again.\n";
            break;
        }

    } while (choice != 4);
    return 0;
}