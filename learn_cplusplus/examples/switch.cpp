#include <cstdio>
enum class Race{
        Dinan,
        Teklan,
        Ivyn,
        Moiran,
        Camite,
        Julian,
        Adian
    };
int main(){
    Race myRace = Race::Dinan;
    switch(myRace){
        case Race::Dinan:
            printf("My race is Dinan\n");
            break;
        case Race::Teklan:
            printf("My race is Teklan\n");
            break;
        case Race::Ivyn:
            printf("My race is Ivyn\n");
            break;
        case Race::Moiran:
            printf("My race is Moiran\n");
            break;
        case Race::Camite:
            printf("My race is Camite\n");
            break;
        case Race::Julian:
            printf("My race is Julian\n");
            break;
        case Race::Adian:
            printf("My race is Adian\n");
            break;
        default:
            printf("Unknown race\n");
    }
}