#include <sys/msg.h> 
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "headers.h"

int client_id;

void sigint(int signo){
    printf("Finishing client process...\n");
    msgctl(client_id, IPC_RMID, NULL);
    exit(0);
}


int self_id = -1;

void client_exit(int id){
    msgctl(id, IPC_RMID, NULL);
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
    printf("Creating key... ");
    key_t key = ftok(getenv("HOME"), SERVER_ID); 
    printf("OK\n");
    printf("Connecting to the main queue... ");
    int server_id = msgget(key, MSGPERM);
    if(server_id < 0){
        printf("%s\n", strerror(errno));
        return 1;
    }
    int end = 0, rc;
    char line[MSGTXTLEN], type[MSGTXTLEN];
    struct msg snd;
    printf("OK\n");


    printf("Creating private queue... ");
    client_id = msgget(IPC_PRIVATE, MSGPERM | IPC_CREAT);
    if(client_id< 0){
        printf("%s\n", strerror(errno));
        return 1;
    }
    printf("OK\n");
    printf("Sending request for accept to server... ");

    snd.msg_type = REQ_CLIENT_START;
    sprintf(snd.msg_text, "%d", client_id);
    rc = msgsnd(server_id, &snd, MSGTXTLEN, 0);
    if(rc < 0){
        printf("%s\n", strerror(errno));
    }
    printf("OK\n");

    rc = msgrcv(client_id, &snd, MSGTXTLEN, 0, 0);
    if(rc < 0){
        printf("%s\n", strerror(errno));
    }
    self_id = atoi(snd.msg_text);
    printf("Received id from server: %d\n", self_id);

    printf("Client initialized\n\n");
    while(!end){
        int send = 1;
        read_stdin(line, type);
        switch(type[0]){
            case 'M':
                snd.msg_type = REQ_MIRROR;
                break;
            case 'C':
                snd.msg_type = REQ_CALC;
                break;
            case 'T':
                snd.msg_type = REQ_TIME;
                break;
            case 'E':
                snd.msg_type = REQ_END;
                break;
            default:
                printf("Unrecognized type: %s\n\n", type);
                send = 0;
        }
        sprintf(snd.msg_text, "%c%s", self_id, line);
        if(send){
            if(msgsnd(server_id, &snd, MSGTXTLEN, 0) < 0){
                printf("%s\n", strerror(errno));
            }
            msgrcv(client_id, &snd, MSGTXTLEN, 0, 0);
            printf("Server response: %s\n", snd.msg_text);
        }
    }
}
