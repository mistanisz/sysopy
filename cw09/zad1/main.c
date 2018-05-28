#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

int p_threads, c_threads, size, L, verbose = 0;
char type;

sem_t producer, *buffer, consumer, counter;
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
        sem_wait(&producer);
        index = (producer_index++)%size;
        sem_post(&producer);
        sem_wait(&buffer[index]);
        if(bufor[index]==0){
            sem_post(&buffer[index]);
            char *line = calloc(100, sizeof(char));
            sem_wait(&producer);
            if(fgets(line, 100, f) == NULL){
                sem_post(&producer);    
                sem_wait(&counter);
                finished++;
                sem_post(&counter);
                break;
            }
            sem_post(&producer);
            sem_wait(&buffer[index]);
            bufor[index] = line;
            sem_post(&buffer[index]);
            sem_wait(&counter);
            filled++;
            sem_post(&counter);
            if(verbose){
                printf("[P] %d:\t%s\n", index, line);
                fflush(stdout);
            }
        }
        else {
            sem_post(&buffer[index]);
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
        sem_wait(&consumer);
        int index=(consumer_index++)%size;
        sem_post(&consumer);
        sem_wait(&buffer[index]);
        if(bufor[index]){      
            line=bufor[index];
            bufor[index]=0;
            sem_post(&buffer[index]);
            printf("[C] %d:\t%s\n", index, line);
            fflush(stdout);
            free(line);
            sem_wait(&counter);
            filled--;
            sem_post(&counter);
        }
        else {
            sem_post(&buffer[index]);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]){
    if(argc!=8){
        printf("Bad number of args\n");
    }
    p_threads = atoi(argv[1]);
    c_threads = atoi(argv[2]);
    size = atoi(argv[3]);
    char *filename = argv[4];
    L = atoi(argv[5]);
    type = argv[6][0];
    verbose = atoi(argv[7]);
    int nk = atoi(argv[8]);
    sem_init(&producer, 0, 1);
    buffer = calloc(size, sizeof(sem_t));
    bufor = calloc(size, sizeof(char*));
    for(int i=0;i<size;++i)sem_init(&buffer[i], 0, 1);
    sem_init(&consumer, 0, 1);
    sem_init(&counter, 0, 1);
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
    sem_destroy(&producer);
    for(int i=0;i<size;++i)sem_destroy(&buffer[i]);
    sem_destroy(&consumer);
    sem_destroy(&counter);
    free(buffer);
    free(bufor);
}
