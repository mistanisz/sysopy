#include "common.c"

sem_t *semid[SEMS];
queue *Q;


void sighandler(int signo){
    log1("Terminating barber process");
    munmap(Q, sizeof(queue));
    shm_unlink(SHARED_NAME);
    for(int i = 0; i< SEMS; ++i){
        sem_close(semid[i]);
    }
}

void init(int chairs){
    char semname[30];
    for(int i = 0; i < SEMS; ++i) {
        sprintf(semname, "/%s%d", SHARED_NAME, i);
        semid[i] = sem_open(semname, O_RDWR | O_CREAT,  0600u, i == SEM_MEM);
        if(semid[i] == SEM_FAILED) {
            char buf[80];
            sprintf(buf, "Failed opening %dth semafore\n", i);
            log_err(buf);
        }
    }
   
    int shmid = shm_open(SHARED_NAME, O_RDWR | O_CREAT, 0600u);
    if(shmid < 0)log_err("Failed to create shared memory");
    if(ftruncate(shmid, sizeof(queue)) < 0 )log_err("Failed to truncate shm");
    Q = mmap(NULL, sizeof(queue), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    if(Q == (void *) -1) log_err("Failed to attach shared memory");

    queue_init(Q, chairs);
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
