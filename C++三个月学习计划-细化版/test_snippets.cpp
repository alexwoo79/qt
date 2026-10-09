#include <iostream>
struct ClassName {
private:
public:
    ClassName() { std::cout << "Constructor called\n"; };
    ~ClassName() { std::cout << "Destructor called\n"; };
};

int main() {
    ClassName obj;
    return 0;
}