#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int p_threads, c_threads, size, L, verbose = 0;
char type;

pthread_mutex_t producer, *buffer, consumer, counter;
int producer_index = 0, consumer_index = 0;
FILE *f;
int finished = 0, filled=0;

char** bufor;

void *producer_thread(void *args){
    while(1){
        if(filled>=size){
            continue;
        }
        int index = -1;
        pthread_mutex_lock(&producer);
        index = (producer_index++)%size;
        pthread_mutex_unlock(&producer);
        pthread_mutex_lock(&buffer[index]);
        if(bufor[index]==0){
            pthread_mutex_unlock(&buffer[index]);
            char *line = calloc(100, sizeof(char));
            pthread_mutex_lock(&producer);
            if(fgets(line, 100, f) == NULL){
                pthread_mutex_unlock(&producer);    
                pthread_mutex_lock(&counter);
                finished++;
                pthread_mutex_unlock(&counter);
                break;
            }
            pthread_mutex_unlock(&producer);
            pthread_mutex_lock(&buffer[index]);
            bufor[index] = line;
            pthread_mutex_unlock(&buffer[index]);
            pthread_mutex_lock(&counter);
            filled++;
            pthread_mutex_unlock(&counter);
            if(verbose){
                printf("[P] %d:\t%s\n", index, line);
                fflush(stdout);
            }
        }
        else {
            pthread_mutex_unlock(&buffer[index]);
        }
    }
    return NULL;
}

void *consumer_thread(void *args){
    while(1){
        if(!filled){
            if(finished==p_threads)break;
            continue;
        }
        char *line=0;
        if(finished)break;
        pthread_mutex_lock(&consumer);
        int index=(consumer_index++)%size;
        pthread_mutex_unlock(&consumer);
        pthread_mutex_lock(&buffer[index]);
        if(bufor[index]){      
            line=bufor[index];
            bufor[index]=0;
            pthread_mutex_unlock(&buffer[index]);
            int l = strlen(line), p=0;
            switch(type){
                case '=':
                    if(l==L)p=1;
                    break;
                case '<':
                    if(l<L)p=1;
                    break;
                case '>':
                    if(l>L)p=1;
                    break;
            }
            if(p)printf("[C] %d:\t%s\n", index, line);
            fflush(stdout);
            free(line);
            pthread_mutex_lock(&counter);
            filled--;
            pthread_mutex_unlock(&counter);
        }
        else {
            pthread_mutex_unlock(&buffer[index]);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]){
    if(argc!=9){
        printf("Bad number of args\n");
        exit(1);
    }
    p_threads = atoi(argv[1]);
    c_threads = atoi(argv[2]);
    size = atoi(argv[3]);
    char *filename = argv[4];
    L = atoi(argv[5]);
    type = argv[6][0];
    verbose = atoi(argv[7]);
    int nk = atoi(argv[8]);
    buffer = calloc(size, sizeof(pthread_mutex_t));
    bufor = calloc(size, sizeof(char*));

    for(int i=0;i<size;++i)pthread_mutex_init(&buffer[i], NULL);
    pthread_mutex_init(&producer, NULL);
    pthread_mutex_init(&consumer, NULL);
    pthread_mutex_init(&counter, NULL);
    f = fopen(filename, "r");
    
    pthread_t ptid[p_threads];
    for(int i=0;i<p_threads;++i){
        pthread_create(&ptid[i], NULL, producer_thread, NULL);
    }
    pthread_t ctid[c_threads];
    for(int i=0;i<c_threads;++i){
        pthread_create(&ctid[i], NULL, consumer_thread, NULL);
    }
    if(nk){
        sleep(nk);
        for(int i=0;i<c_threads;++i) pthread_cancel(ctid[i]);
        for(int i=0;i<p_threads;++i) pthread_cancel(ptid[i]);
    }
    else {
        for(int i=0;i<c_threads;++i) pthread_join(ctid[i], NULL);
        for(int i=0;i<p_threads;++i) pthread_join(ptid[i], NULL);
    }
    fclose(f);
    free(buffer);
    free(bufor);
}
