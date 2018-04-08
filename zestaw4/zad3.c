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


pid_t pid;
int c_send_p = 0, c_rec_c = 0, c_rec_p = 0;

void fun_p(int signo){
    c_rec_p++;
}

void fun_c(int signo){
    c_rec_c++;
    kill(getppid(), signo);
}

void fun_c1(int signo){
    printf("Signals received child: %d\n", c_rec_c);
    exit(0);
}

void sigint(int signo){
    printf("Received SIGINT, killing %d\n", pid);
    kill(pid, SIGUSR2);
    exit(0);
}

int main(int argc, char *argv[]){
    if(argc<3){
        printf("Not enough arguments\n");
        return -1;
    }
    int L = atoi(argv[1]), type = atoi(argv[2]);

    sigset_t newmask, oldmask;

    pid = fork();
    if(!pid){
        sigfillset(&newmask);
        sigdelset(&newmask, SIGUSR1);
        sigdelset(&newmask, SIGUSR2);
        sigdelset(&newmask, SIGRTMIN);
        sigdelset(&newmask, SIGRTMAX);
        sigprocmask(SIG_SETMASK, &newmask, &oldmask);
        signal(SIGUSR1, &fun_c);
        signal(SIGUSR2, &fun_c1);
        signal(SIGRTMIN, &fun_c);
        signal(SIGRTMAX, &fun_c1);
        while(1)sleep(1);
    }
    else{
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        signal(SIGUSR1, &fun_p);
        signal(SIGRTMIN, &fun_p);
        signal(SIGINT, &sigint);
        int i;
        sleep(1);
        switch(type){
            case 1:
                for(i = 0;i<L;++i){
                    kill(pid, SIGUSR1);
                    c_send_p++;
                }
                printf("Signals sent: %d\n", c_send_p);
                kill(pid, SIGUSR2);
                break;
            case 2:
                sigprocmask(SIG_BLOCK, &newmask, &oldmask);
                for(i = 0;i<L;++i){
                    kill(pid, SIGUSR1);
                    c_send_p++;
                    sigsuspend(&oldmask);
                }
                printf("Signals sent: %d\n", c_send_p);
                kill(pid, SIGUSR2);
                break;
            case 3:
                for(i = 0;i<L;++i){
                    kill(pid, SIGRTMIN);
                    c_send_p++;
                }
                printf("Signals sent: %d\n", c_send_p);
                kill(pid, SIGRTMAX);
                break;
            default:
                printf("Unrecognized type\n");
                kill(pid, SIGINT);
                return -1;
        }
        wait(NULL);
        printf("Singals received parent: %d\n", c_rec_p);
    }
}
