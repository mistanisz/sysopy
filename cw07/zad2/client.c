#include "common.c"

sem_t *semid[SEMS];
queue *Q;

void leave_shop(int x){
    if(x)log1("Leaving shop with new haircut");
    else log1("Leaving the shop, no seats in waiting room");
}

void cut(){
    log1("Sitting on chair");
    sem_free(semid, SEM_WORK);
    sem_block(semid, SEM_WORK1);
    leave_shop(1);
}

void client_wait(){
    sem_block(semid, SEM_WAIT);
    sem_block(semid, SEM_MEM);
    int x = Q->next;
    sem_free(semid, SEM_MEM);
    if(x == getpid()) return cut();
    sem_free(semid, SEM_WAIT);
    return client_wait();
}

void enter_shop(){
    sem_block(semid, SEM_MEM);
    if(Q->is_sleeping){
        Q->next = getpid();
        Q->is_sleeping = 0;
        sem_free(semid, SEM_MEM);
        log1("Waking barber");
        sem_free(semid, SEM_WAIT);
        return cut();
    }
    else if(queue_is_full(Q)){
        sem_free(semid, SEM_MEM);
        return leave_shop(0);
    }
    else {
        queue_push(getpid(), Q);
        sem_free(semid, SEM_MEM);
        log1("Sitting on the chair in waiting room");
        return client_wait();
    }
}


void init(){
    char semname[30];
    for(int i = 0; i < SEMS; ++i) {
        sprintf(semname, "/%s%d", SHARED_NAME, i);
        semid[i] = sem_open(semname, O_RDWR,  0600u, i == SEM_MEM);
        if(semid[i] == SEM_FAILED) {
            char buf[80];
            sprintf(buf, "Failed opening %dth semafore\n", i);
            log_err(buf);
        }
    }
   
    int shmid = shm_open(SHARED_NAME, O_RDWR, 0600u);
    if(shmid < 0)log_err("Failed to create shared memory");
    if(ftruncate(shmid, sizeof(queue)) < 0 )log_err("Failed to truncate shm");
    Q = mmap(NULL, sizeof(queue), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    if(Q == (void *) -1) log_err("Failed to attach shared memory");
}

int main(int argc, char* argv[]){
    if(argc != 3) log_err("Bad number of arguments");
    int clients = atoi(argv[1]);
    int repeats = atoi(argv[2]);
    init();
    
    for(int i=0;i<clients; ++i){
        pid_t pid = fork();
        if(!pid) {
            while(repeats--)enter_shop();
            exit(0);
        }
    }
    while(clients--)
        wait(NULL);
}
