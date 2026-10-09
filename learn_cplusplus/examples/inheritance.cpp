#include <iostream>
class Entity {
  public:
    Entity() {
        x_ = 0.0f;
        y_ = 0.0f;
        std::cout << "Created Entity" << std::endl;
    }
    ~Entity() { std::cout << "Destructed Entity" << std::endl; }
    Entity(float x, float y) : x_(x), y_(y) {}
    void PrintPosition() {
        std::cout << "x: " << x_ << ", y: " << y_ << std::endl;
    }

  private:
    float x_;
    float y_;
};
int main() {
    Entity e{1.0f, 1.0f};
    e.PrintPosition();
    std::cin.get();
}