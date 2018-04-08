#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


int rec = 0, state = 0;
int K=0, j=0, *T;
pid_t *t;

void send(pid_t pid){
    printf("Send permission to PID %d\n", pid);
    kill(pid, SIGUSR2);
}

void fun(int signo, siginfo_t *info, void *extra){
    if(signo == SIGUSR1){
        printf("Got request from PID %d\n", info->si_pid);
        if(!state) T[rec++] = info->si_pid;
        if(!state && rec >= K){
            state = 1;
            int i=0;
            for(;i<K;++i)send(T[i]);
        }
        else if(state)send(info->si_pid);
    }
    else if(signo == SIGUSR2){}
    else if(signo >= SIGRTMIN && signo <= SIGRTMAX){
        printf("Received real time signal %d from PID %d\n", signo, info->si_pid);
    }
}

int main(int argc, char *argv[]){
    int i;
    struct sigaction usr1;
    usr1.sa_sigaction = &fun;
    sigemptyset(&usr1.sa_mask);
    usr1.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &usr1, NULL);
    sigaction(SIGUSR2, &usr1, NULL);
    for(i = SIGRTMIN;i<=SIGRTMAX;++i)
        sigaction(i, &usr1, NULL);

    sigset_t newmask, oldmask;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR2);

    if(argc<3){
        printf("Not enough arguments\n");
        return -1;
    }
    int N = atoi(argv[1]);
    K = atoi(argv[2]);
    pid_t m_pid = getpid();
    t = calloc(N, sizeof(pid_t));
    T = calloc(K, sizeof(pid_t));
    for(i = 0; i < N; ++i){
        if(getpid() == m_pid){
            t[i] = fork();
            if(t[i])printf("Created process %d\n", t[i]);
        }
    }
    if(getpid() != m_pid){
        sigprocmask(SIG_BLOCK, &newmask, &oldmask);
        srand(getpid());
        int s = rand()%11;
        sleep(s);
        kill(getppid(), SIGUSR1);
        sigsuspend(&oldmask);
        int n = rand()%32;
        kill(getppid(), SIGRTMIN+n);
        exit(s);
    }
    int s;
    pid_t pid;
    while(1){
        pid = wait(&s);
        if (errno == ECHILD) {
            break;
        }    
        if(pid>0)printf("Process %d exited with status code: %d\n", pid, s);
    }
    free(t);
    free(T);
}
