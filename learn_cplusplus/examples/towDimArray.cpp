#include <iostream>

using namespace std;

int main() {
    // cars[rows][cols]
    std::string cars[][3] = {
        {"Mustang", "Camaro", "Challenger"}, {"Civic", "Accord", "Fit"}, {"Corolla", "Camry", "Prius"}};
    int rows = sizeof(cars) / sizeof(cars[0]);
    int cols = sizeof(cars[0]) / sizeof(cars[0][0]);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << cars[i][j] << ",";
        }
        cout << '\n';
    }
}
\\