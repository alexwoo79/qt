#include <iostream>

using namespace std;

int main() {
    std::string foods[5];
    int size = sizeof(foods) / sizeof(foods[0]);

    std::string temp;

    for (int i = 0; i < size; i++) {
        cout << "Enter food you like or 'q' to quit: " << i + 1 << ": ";
        std::getline(cin, temp);
        if (temp == "q") {
            break;
        } else {
            foods[i] = temp;
        }
    }
    cout << "\nYou liked food:\n";
    // Print only the foods that were entered, stopping at the first empty entry.
    for (int i = 0; !foods[i].empty(); i++) {
        cout << "Food item " << i + 1 << ": " << foods[i] << '\n';
    }
    return 0;
}
