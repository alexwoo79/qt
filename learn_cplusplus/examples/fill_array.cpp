#include <iostream>

using namespace std;

int main() {
    // std::string foods[10] = {"pizza", "burger",   "pasta", "salad", "sushi",
    //                          "tacos", "sandwich", "steak", "fries", "ice cream"};

    // for (std::string food : foods) {
    //     cout << food << '\n';
    // }
    // cout << endl;
    // fill(begin,end,value)
    const int SIZE = 9;
    std::string foods[SIZE];
    fill(foods, foods + SIZE / 3, "pizza");
    fill(foods + SIZE / 3, foods + 2 * SIZE / 3, "burger");
    fill(foods + 2 * SIZE / 3, foods + SIZE, "pasta");

    for (std::string food : foods) {
        cout << food << '\n';
    }
    return 0;
}
