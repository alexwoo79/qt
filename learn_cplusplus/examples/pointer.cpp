#include <cstdio>
int main() {
    int gettysburg{};
    printf("Gettysburg: %d\n", gettysburg);
    int* gettysburg_address = &gettysburg;
    printf("Pointer to Gettysburg: %p\n", gettysburg_address); 

    *gettysburg_address=17325;
    printf("Gettysburg after update: %d\n", gettysburg);
    printf("Pointer to Gettysburg after update: %p\n", gettysburg_address);
}