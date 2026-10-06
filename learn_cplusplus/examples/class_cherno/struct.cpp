#include <iostream>

#define LOG(x) std::cout << x << std::endl;// define a macro for logging values to the console

struct Player{
// public: //struct default is public
    int x,y;
    int speed;
    void Move(int xa,int ya) {
        x += xa * speed;
        y += ya * speed;
    }
    private: // struct members are public by default, but you can still have private members if needed
    int health;
};

//Cherno's C++ normally use struct for simple data structures with public members by default
//Using Class for more complex data structures with private members and encapsulation

int main() {
    Player player_struct;
    player_struct.x = 5;
    player_struct.y = 0;
    player_struct.speed = 5;   
    
    player_struct.Move(1, 1); // Move the player by 1 unit in both x and y directions
    
    LOG(player_struct.x);
    LOG(player_struct.y);

    return 0;
}