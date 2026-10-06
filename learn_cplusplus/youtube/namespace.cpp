#include <iostream>

namespace First{
    int x =1;
}
namespace Second{
    int x = 2;
}

int main() {
    using namespace First;
    using namespace std;
    // using namespace Second;
    cout << "Hello, namespace!" << endl;
    cout << "First::x = " << x << endl;
    // cout << "Second::x = " << Second::x << endl;
}