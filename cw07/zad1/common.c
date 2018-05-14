#include "common.h"
#include <time.h>

void log1(const char* S){
    struct timespec time; \
    clock_gettime(CLOCK_MONOTONIC, &time); \
    printf("%06ld PID: %d -> %s\n", time.tv_nsec/1000, getpid(), S);
    fflush(stdout);
}

void log_err(const char* S){
    fprintf(stderr, "%s: %s\n", S, strerror(errno));
    exit(1);
}

void queue_push(int x, queue *Q){
    if(queue_is_full(Q))log_err("Queue full");
    Q->t[(Q->curr+Q->elems)%QUEUE_SIZE] = x;
    Q->elems++;
}

int queue_pop(queue *Q){
    if(queue_is_empty(Q))log_err("Queue epmty");
    Q->elems--;
    int x = Q->t[Q->curr];
    Q->curr=(Q->curr+1)%QUEUE_SIZE;
    return x;
}

void queue_init(queue *Q, int chairs){
    Q->elems = 0;
    Q->curr = 0;
    Q->is_sleeping = 0;
    Q->next = 0;
    Q->chairs = chairs;
}

int queue_is_full(queue *Q){
    return (Q->elems == Q->chairs);
}

int queue_is_empty(queue *Q){
    return (Q->elems == 0);
}

void sem_op(int semid, int semnum, int val){
    struct sembuf buf = {
            .sem_op = val,
            .sem_num = semnum,
            .sem_flg = 0
    };
    if(semop(semid, &buf, 1) < 0) log_err("Semaphore operation failed");
}

void sem_block(int semid, int semnum){
    sem_op(semid, semnum, -1);
}

void sem_free(int semid, int semnum){
    sem_op(semid, semnum, 1);
}

key_t get_key(void){
    return ftok(getenv("HOME"), 8);
}
