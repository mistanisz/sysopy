#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


int main(int argc, char *argv[]){
    if(argc<2){
        printf("Not enough arguments\n");
        return -1;
    }
    mkfifo(argv[1], 0666);
    FILE *stream = fopen(argv[1], "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stream)) != -1)
        printf("%s", line);

    fclose(stream);
    unlink(argv[1]);
}
