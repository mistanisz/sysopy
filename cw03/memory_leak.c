#include<stdlib.h>
#include <unistd.h>

int main(){
    int *i;
    while(1){
        i = (int*)calloc(1024, sizeof(int));
        i[1023] = -1;
    }
    return 0;
}
