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
int state = 0;

void sigint(int signo){
    printf("\nOdebrano sygnał SIGINT\n");
    kill(-getpgid(pid), SIGINT);
    exit(0);
}


void run(){
    setpgrp();
    execl("zad1/date.sh", "date.sh", NULL);
}

void sigtstp(int signo){
    if(!state){
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
        state = 1;
        kill(-getpgid(pid), SIGINT);
    }
    else{
        pid = fork();
        if(!pid) run();
        state = 0;
    }
}

int main(){
    struct sigaction act;
    act.sa_handler = sigtstp;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    signal(SIGINT, &sigint);
    sigaction(SIGTSTP, &act, NULL);
    pid = fork();
    if(!pid) run();
    while(1)sleep(1);
}
