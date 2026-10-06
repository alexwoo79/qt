#include <iostream>

int main() {
    char* buffer =new char[8];
    memset(buffer, 0, 8); // Initialize the buffer with zeros
    
    char** ptr = &buffer;
    
    delete[] buffer;
    return 0;
}