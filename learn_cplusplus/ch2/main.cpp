#include <cstdio>
int main(){
    // unsigned int a = 3669732608;
    // printf("Yabba %x!\n", a);
    // unsigned int b=69;
    // printf("There are %u,%o leaves here!\n", b, b);

    double an = 9.22e23;
    printf("The value of an is %le %lf %lg\n", an, an, an);
    float hp = 9.75;
    printf("The value of hp is %.2e %.2f %g\n", hp, hp, hp);
    //%g used to show the value in either %f or %e format, whichever is shorter

    bool b1 = true;
    printf("The value of b1 is %d\n", b1);
    bool b2 = false;
    printf("The value of b2 is %d\n", b2);
    bool b3 = b1 && b2;
    printf("The value of b3 (b1 && b2) is %d\n", b3);
    bool b4 = b1 || b2;
    printf("The value of b4 (b1 || b2) is %d\n", b4);   
    printf("7==7 is %d\n", 7==7);
    printf("7!=7 is %d\n", 7!=7);
    printf("7>5 is %d\n", 7>5);
    printf("7<5 is %d\n", 7<5);
    printf("7>=5 is %d\n", 7>=5);
    printf("7<=5 is %d\n", 7<=5);

}