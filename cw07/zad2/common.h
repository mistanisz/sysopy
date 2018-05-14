#define QUEUE_SIZE 1000

#define SEMS 4
#define SEM_MEM 0
#define SEM_WORK 1
#define SEM_WORK1 3
#define SEM_WAIT 2

#define SHARED_NAME "/meiso"

#include <semaphore.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct queue{
    int t[QUEUE_SIZE];
    int curr;
    int elems;
    int is_sleeping;
    int next;
    int chairs;
} queue;

void queue_init(queue *Q, int);
void queue_push(int x, queue *Q);
int queue_pop(queue *Q);
int queue_is_full(queue *Q);
int queue_is_empty(queue *Q);

void sem_block(sem_t *semid[], int semnum);
void sem_free(sem_t *semid[], int semnum);

void log1(const char*);
void log_err(const char* S);
