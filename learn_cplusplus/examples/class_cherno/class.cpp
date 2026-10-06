#include <iostream>

#define LOG(x) std::cout << x << std::endl;// define a macro for logging values to the console

class Player{
public:
    int x,y;
    int speed;
    void Move(int xa,int ya) {
        x += xa * speed;
        y += ya * speed;
    }
};

int main() {
    Player player;
    player.x = 5;
    player.y = 0;
    player.speed = 5;   
    
    player.Move(1, 1); // Move the player by 1 unit in both x and y directions
    
    LOG(player.x);
    LOG(player.y);
    std::cin.get();
    return 0;
}