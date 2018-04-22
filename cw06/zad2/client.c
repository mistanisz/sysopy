#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <mqueue.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/resource.h>
#include "headers.h"

mqd_t client_id = -1;
int self_id = -1;
char name[80];

void sigint(int signo){
    printf("Finishing client process...\n");
    mqd_t server_id = mq_open(SERVERNAME, O_WRONLY);
    char message[MSGTXTLEN];
    sprintf(message, "%c%c", REQ_STOP, self_id);
    mq_send(server_id, message, MSGTXTLEN, MSGPRIO);
    mq_close(server_id);
    mq_unlink(name);
    exit(0);
}

void read_stdin(char *text, char *type){
    char buf[256];
    if (fgets(buf, MSGTXTLEN, stdin) == NULL)
        perror("fgets");

    char *str = strstr(buf, " ");
    if(str != NULL){
        *str = 0;
        strcpy(type, buf);
        str++;
        strcpy(text, str);
        text[strlen(text)-1]=0;
    }
    else {
        strcpy(type, buf);
        text[0] = 0;
    }
}

int main(){
    signal(SIGINT, &sigint);

    printf("Initalizing client...\n");
    printf("Connecting to the main queue... ");
    mqd_t server_id = mq_open(SERVERNAME, O_WRONLY);
    if(server_id < 0){
        printf("%s\n", strerror(errno));
        return 1;
    }
    printf("OK\n");
    int end = 0;
    char line[MSGTXTLEN], type[MSGTXTLEN];
    char message[MSGTXTLEN];


    printf("Creating private queue... ");
    while(client_id < 0){ 
        sprintf(name, "/client%ld", time(NULL));
        client_id = mq_open(name, O_RDONLY | O_CREAT | O_EXCL, MSGPERM , NULL);
        if(client_id< 0){
            printf("%s\n", strerror(errno));
        }
    }
    printf("OK\n");
    printf("Sending request for accept to server... ");

    sprintf(message, "%c%s", REQ_CLIENT_START, name);
    if(mq_send(server_id, message, MSGTXTLEN, MSGPRIO) < 0){
        printf("%s\n", strerror(errno));
    }
    printf("OK\n");

    if(mq_receive(client_id, message, MSGTXTLEN, NULL) < 0){
        printf("%s\n", strerror(errno));
    }
    self_id = atoi(message);
    printf("Received id from server: %d\n", self_id);

    printf("Client initialized\n\n");
    while(!end){
        int t = 0, prio = MSGPRIO;
        read_stdin(line, type);
        switch(type[0]){
            case 'M':
                t = REQ_MIRROR;
                break;
            case 'C':
                t = REQ_CALC;
                break;
            case 'T':
                t = REQ_TIME;
                break;
            case 'E':
                t = REQ_END;
                prio--;
                break;
            default:
                printf("Unrecognized type: %s\n\n", type);
                t = 0;
        }
        sprintf(message, "%c%c%s", t, self_id, line);
        if(t){
            if(mq_send(server_id, message, MSGTXTLEN, prio) < 0){
                printf("%s\n", strerror(errno));
            }
            mq_receive(client_id, message, MSGTXTLEN, NULL);
            printf("Server response: %s\n", message);
        }
    }
}
