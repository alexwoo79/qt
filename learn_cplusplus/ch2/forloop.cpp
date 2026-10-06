#include <cstddef>
#include <cstdio>

int main(){
    unsigned long maxium =0;
    unsigned long values[]{10,50,20,40,0};
    // for(size_t i=0;i<sizeof(values)/sizeof(values[0]);i++){
    //     if(values[i]>maxium){
    //         maxium=values[i];
    //     }
    // }
    for (unsigned long value : values){
        if(value > maxium){
            maxium = value;
        }
    }
    printf("The maximum value is %lu\n", maxium);

}