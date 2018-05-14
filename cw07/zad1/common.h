#define QUEUE_SIZE 1000

#define SEMS 4
#define SEM_MEM 0
#define SEM_WORK 1
#define SEM_WORK1 3
#define SEM_WAIT 2


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

void sem_block(int semid, int semnum);
void sem_free(int semid, int semnum);

void log1(const char*);
void log_err(const char* S);
