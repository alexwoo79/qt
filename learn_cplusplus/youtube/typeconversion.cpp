#include<iostream>

int main() {
    
    //type conversion example
    // double x = (int)3.14;

    // char x=100;
    int correct = 8;
    int questions = 10;
    double score = correct / (double)questions * 100;
    std::cout << "Score: " << score << "%" << std::endl;

    return 0;
}