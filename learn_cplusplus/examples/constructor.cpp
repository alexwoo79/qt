#include <iostream>

class Entity {
  public:
    void Move(float dx, float dy) {
        x_ += dx;
        y_ += dy;
    }

    void PrintPosition() {
        std::cout << "x: " << x_ << ", y: " << y_ << std::endl;
    }
    // Constructors for the Entity class
    Entity() : x_(0.0f), y_(0.0f) {}

    Entity(float x, float y) : x_(x), y_(y) {}

  private:
    float x_; // member variable required for storing the x-coordinate
    float y_; // member variable required for storing the y-coordinate
};

class Player : public Entity {
    // Constructors for the Player class
  public:
    const char *name_;
    void PrintName() { std::cout << "Name: " << name_ << std::endl; }
};

// main function
int main() {
    Player p;
    p.name_ = "John";
    p.PrintName();
    p.Move(5.0f, 3.0f);
    p.PrintPosition();

    std::cout << sizeof(Entity) << std::endl;
    std::cout << sizeof(Player) << std::endl;
    std::cin.get();
}