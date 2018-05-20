#include<stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
//    if(argc != 2) log_err("Bad number of arguments");
    char line[100];
    float val;
    FILE *filter_fd = fopen(argv[1], "r");
    int c = 0;
    if(fgets(line, sizeof(line), filter_fd)){
        c = atoi(line);
        printf("C: %d\n", c);
    }
//    if (c == 0) log_err("Bad filter");
    float **filter = calloc(c, sizeof(float*));
    for(int i =0;i<c;++i){
        filter[i] = calloc(c, sizeof(float));
    }
    int i = 0, j = 0;
    while(!feof(filter_fd)) {
        if(fgets(line, sizeof(line), filter_fd)){
            val = atof(line);
            filter[i][j++] = val;
            if(j == c){
                i++;
                j = 0;
            }
        }
    }
    fclose(filter_fd);
    for(int i=0;i<c;++i){
        for(int j=0;j<c;++j){
            printf("%f ", filter[i][j]);
        }    
        printf("\n");
    }
    for(int i=0;i<c;++i)free(filter[i]);
    free(filter);
}
