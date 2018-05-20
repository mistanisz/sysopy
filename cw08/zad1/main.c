#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

float **filter;
int **image;
int **output;

typedef struct im{
    int width;
    int height;
} image_str;

typedef struct thread_info{
    int width;
    int height;
    int begin;
    int end;
    int c;
} thread_info;

void log_err(const char* S){
fprintf(stderr, "%s: %s\n", S, strerror(errno));
exit(1);
}

int open_filter(char *filename){
    char line[100];
    float val;
    FILE *filter_fd = fopen(filename, "r");
    int c = 0;
    if(fgets(line, sizeof(line), filter_fd)){
        c = atoi(line);
    }
    if (c == 0) log_err("Bad filter");
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

void print(int width, int height, char* filename){
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "P2\n%d %d\n255\n", width, height);
    int a=0;
    for(int i=0;i<height;++i){
        for(int j=0;j<width;++j){
            fprintf(fp, "%d  ", output[i][j]);
            if(++a==12){
                fprintf(fp, "\n");
                a=0;
            }
        }
    }
}

void *apply_filter(void * arg){
    thread_info *ti = (thread_info *) arg;
    for(int i=0;i<ti->height;++i)
        for(int j=ti->begin;j<ti->end;++j)
            output[i][j] = count(i, j, ti->c, ti->width, ti->height);        
    free(ti);
}

int main(int argc, char *argv[]){
    if(argc != 5) log_err("Bad number of arguments");
    image_str im= open_image(argv[2]);
    int c = open_filter(argv[3]);
    int threads = atoi(argv[1]);

    output = calloc(im.width, sizeof(int*));
    for(int i =0;i<im.height;++i){
        output[i] = calloc(im.width, sizeof(int));
    }

    struct timeval begin, end, res;
    pthread_t *tid = calloc(threads, sizeof(pthread_t));
    gettimeofday(&begin, NULL);
    for(int i=0;i<threads;++i){
        thread_info *ti = malloc(sizeof(thread_info));
        ti->width = im.width;
        ti->height = im.height;
        ti->c = c;
        int diff = im.width/threads;
        ti->begin = diff*i;
        ti->end = i<threads-1 ? ti->begin+diff : ti->width;
        pthread_create(&tid[i], NULL, apply_filter, (void *)ti);
    }
    for(int i=0;i<threads;++i)pthread_join(tid[i], NULL);
    gettimeofday(&end, NULL);
    timersub(&end, &begin, &res);
    printf("Applying filter with %d thread%s took %ld.%ld\n", threads, threads==1 ? "" : "s", res.tv_sec, res.tv_usec);
    print(im.width, im.height, argv[4]);

    free(tid);
    for(int i=0;i<c;++i)free(filter[i]);
    free(filter);
    for(int i=0;i<im.height;++i)free(image[i]);
    free(image);
    for(int i=0;i<im.height;++i)free(output[i]);
    free(output);
}
