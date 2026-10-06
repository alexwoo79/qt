#include <cstdio>
struct Book {
    char name[256];
    int year;
    int pages;
    bool hardcover;
};
int main(){
    // Book myBook = {"The Great Gatsby", 1925, 218, true};
    // printf("Book name: %s\n", myBook.name);
    // printf("Year: %d\n", myBook.year);
    // printf("Pages: %d\n", myBook.pages);
    // printf("Hardcover: %s\n", myBook.hardcover ? "Yes" : "No");
    // return 0;
    //using  dot notation to access members of the struct
    Book myBook;
    myBook.year = 1925;
    myBook.pages = 218;
    myBook.hardcover = true;
    snprintf(myBook.name, sizeof(myBook.name), "The Great Gatsby");
}