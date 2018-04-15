#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]){
    if(argc<3){
        printf("Not enough arguments\n");
        return -1;
    }
    printf("PID: %d\n", getpid());
    int N = atoi(argv[2]), i;
    char *buf = calloc(100, sizeof(char));
    srand(8);
    for(i = 0;i < N;++i){
        FILE *stream = fopen(argv[1], "w");
        FILE *f = popen("date", "r");
        fgets(buf, 100, f);
        pclose(f);
        fprintf(stream, "PID: %d, Date: %s\n", getpid(), buf);
        sleep(rand()%5+1);
        fclose(stream);
    }
    free(buf);
}
