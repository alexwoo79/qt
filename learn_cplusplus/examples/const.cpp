#include <iostream>

int main() {

   const double PI = 3.14159;
   const int RADIUS = 5;
   // pi = 23; // This line will now cause a compilation error because pi is const
   double radius = RADIUS;
   double circumference = 2 * PI * radius;
   
   std::cout << "The circumference of the circle is: " << circumference << std::endl;
   return 0;
}