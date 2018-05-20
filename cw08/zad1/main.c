#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

float **filter;
int **image;
int **output;

typedef struct im{
    int width;
    int height;
} image_str;

int open_filter(char *filename){
    char line[100];
    float val;
    FILE *filter_fd = fopen(filename, "r");
    int c = 0;
    if(fgets(line, sizeof(line), filter_fd)){
        c = atoi(line);
    }
//    if (c == 0) log_err("Bad filter");
    filter = calloc(c, sizeof(float*));
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
    return c;
}

image_str open_image(char *filename){
    char line[100];
    int val;
    FILE *image_fd = fopen(filename, "r");
    int c = 0;
    fgets(line, sizeof(line), image_fd);
    //TODO add check if proper file
    image_str ret;
    fgets(line, sizeof(line), image_fd);
    while(line[0] == '#'){
        fgets(line, sizeof(line), image_fd);
    }
    char *tok = strtok(line, " \n\r");
    ret.width = atoi(tok);
    tok = strtok(NULL, " \n\r");
    ret.height = atoi(tok);
    fgets(line, sizeof(line), image_fd);
    image = calloc(ret.width, sizeof(int*));
    for(int i =0;i<ret.height;++i){
        image[i] = calloc(ret.width, sizeof(int));
    }
    int i = 0, j = 0;
    while(!feof(image_fd)) {
        if(fgets(line, sizeof(line), image_fd)){
            char *tok = strtok(line, " \n\r");
            while(tok != NULL){
                val = atoi(tok);
                image[i][j++] = val;
                tok = strtok(NULL, " \n\r");
                if(j == ret.width){
                    i++;
                    j = 0;
                }
            }
        }
    }
    fclose(image_fd);
    return ret;
}

int max(double x, double y){
    return x > y ? x : y;
}

int min(double x, double y){
    return x < y ? x : y;
}

int count(int x, int y, int c, int width, int height){
    double sum = 0;
    for(int i=0;i<c;++i)
        for(int j=0;j<c;++j){
            int a = max(0, min(height - 1, x - ceil(c/2) + i));
            int b = max(0, min(width - 1, y - ceil(c/2) + j));
            sum += image[a][b] * filter[i][j];
        }
    return round(sum);
}

void print(int width, int height){
    int a=0;
    for(int i=0;i<height;++i){
        for(int j=0;j<width;++j){
            printf("%d  ", output[i][j]);
            if(++a==12){
                printf("\n");
                a=0;
            }
        }
    }
}

int main(int argc, char *argv[]){
//    if(argc != 2) log_err("Bad number of arguments");
    image_str im= open_image(argv[1]);
    int c = open_filter(argv[2]);

    output = calloc(im.width, sizeof(int*));
    for(int i =0;i<im.height;++i){
        output[i] = calloc(im.width, sizeof(int));
    }

    for(int i=0;i<im.height;++i){
        for(int j=0;j<im.width;++j){
            output[i][j] = count(i, j, c, im.width, im.height);        
        }
    }
    print(im.width, im.height);

    for(int i=0;i<c;++i)free(filter[i]);
    free(filter);
    for(int i=0;i<im.height;++i)free(image[i]);
    free(image);
    for(int i=0;i<im.height;++i)free(output[i]);
    free(output);
}
