#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "common.c"

int semid;
queue *Q;


void sighandler(int signo){
    log1("Terminating barber process");
    semctl(semget(get_key(), 0, 0600u), 0, IPC_RMID);
    shmdt(Q);
    shmctl(shmget(get_key(), sizeof(queue), 0600u), IPC_RMID, NULL);
}

void init(int chairs){
    key_t key = get_key();
    semid = semget(key, SEMS, IPC_CREAT | 0600u);
    if(semid < 0)log_err("Failed to create semaphore set");
    int shmid = shmget(key, sizeof(queue), IPC_CREAT | 0600u);
    if(shmid < 0)log_err("Failed to create shared memory");
    Q = shmat(shmid, NULL, 0);
    if(Q == (void *) -1) log_err("Failed to attach shared memory");

    queue_init(Q, chairs);
    semctl(semid, SEM_MEM, SETVAL, 1);
    semctl(semid, SEM_WORK, SETVAL, 0);
    semctl(semid, SEM_WAIT, SETVAL, 0);
}

void next_client();

void barber_work(){
    sem_block(semid, SEM_MEM);
    int x = Q-> next;
    sem_free(semid, SEM_MEM);
    char buf[80];
    sem_block(semid, SEM_WORK);
    sprintf(buf, "Starting cutting %d", x);
    log1(buf);
    sprintf(buf, "Finished cutting %d", x);
    log1(buf);
    sem_free(semid, SEM_WORK1);
    return next_client();
}

void barber_sleep(){
    log1("Going to sleep");
    Q->is_sleeping = 1;
    sem_free(semid, SEM_MEM);
    sem_block(semid, SEM_WAIT);
    log1("Waking up");
    return barber_work();
}

void next_client(){
    sem_block(semid, SEM_MEM);
    if(queue_is_empty(Q)){
        return barber_sleep();
    }
    else {
        int x = queue_pop(Q);
        sem_free(semid, SEM_MEM);
        char buf[80];
        sprintf(buf, "Inviting %d", x);
        log1(buf);
        sem_block(semid, SEM_MEM);
        Q->next = x;
        sem_free(semid, SEM_MEM);
        sem_free(semid, SEM_WAIT);
        return barber_work();
    }
}

int main(int argc, char *argv[]){
    if(argc != 2) log_err("Bad number of arguments");
    int chairs = atoi(argv[1]);
    signal(SIGTERM, &sighandler);
    init(chairs);
    next_client();
}
