    int guess{0};
    
    while (guess != num) {
        std::cout << "Enter a guess between (1-100): ";
        std::cin >> guess;
        ++tries;
    
        if (guess > num) {
            std::cout << "Too high!\n";
        } else if (guess < num) {
            std::cout << "Too low!\n";
        } else {
            std::cout << "Congratulations! You guessed the number in "
                      << tries << " tries!\n";
        }
    }