#include <sys/msg.h> 
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "headers.h"

int clients[100];
int client_id = 0;

void sigint(int signo){
    printf("Received SIGINT signal\n");
}

void server_exit(int id){
    printf("Finishing server process...\n");
    msgctl(id, IPC_RMID, NULL);
}

int start_client(int id, int q_id){
    struct msg message;
    printf("Received client request\n");
    message.msg_type = SERVER_RESPONSE;
    sprintf(message.msg_text, "%d", client_id);
    if(msgsnd(id, &message, MSGTXTLEN, 0) < 0){
        printf("Client not accepted: ");
        printf("%s\n", strerror(errno));
        return 0;
    }
    printf("Client accepted with id: %d\n",client_id);
    return 1;
}

void handle(struct msg message, int q_id, char *cmd){
    int id, i = message.msg_text[0];
    id = clients[i];
    printf("%d\n", i);

    FILE *f = popen(cmd, "r");
    fgets(message.msg_text, 100, f);
    message.msg_type = SERVER_RESPONSE;
    pclose(f);
    if(msgsnd(id, &message, MSGTXTLEN, 0) < 0){
        printf("Not responded: ");
        printf("%s\n", strerror(errno));
    }
}

void handle_mirror(struct msg message, int q_id){
    printf("Received MIRROR request from: ");
    char cmd[80];
    sprintf(cmd, "echo %s | rev", message.msg_text+1);
    handle(message, q_id, cmd);
}

void handle_time(struct msg message, int q_id){
    printf("Received TIME request from: ");
    char cmd[80];
    sprintf(cmd, "date");
    handle(message, q_id, cmd);
}

void handle_calc(struct msg message, int q_id){
    printf("Received CALC request from: ");
    char cmd[80];
    sprintf(cmd, "echo %s | bc", message.msg_text+1);
    handle(message, q_id, cmd);
}

int main(){
    signal(SIGINT, &sigint);
    printf("Starting server...\n");
    printf("Creating key... ");
    key_t key = ftok(getenv("HOME"), SERVER_ID); 
    printf("OK\n");
    printf("Creating messsage queue... ");
    int msg_id = msgget(key, MSGPERM | IPC_CREAT);
    printf("OK\n");
    struct msg recv;
    int flag = 0;
    printf("Server initalized\n\n");
    while(2){
        if(msgrcv(msg_id, &recv, MSGTXTLEN, 0, flag) < 0){
            server_exit(msg_id);
            break;
        }
        switch(recv.msg_type){
            case REQ_CLIENT_START:
                clients[client_id] = atoi(recv.msg_text);
                if(start_client(clients[client_id], msg_id))client_id++;
                break;
            case REQ_MIRROR:
                handle_mirror(recv, msg_id);
                break;
            case REQ_TIME:
                handle_time(recv, msg_id);
                break;
            case REQ_CALC:
                handle_calc(recv, msg_id);
                break;
            case REQ_END:
                printf("Received END request\n");
                flag |= IPC_NOWAIT;
                break;
            default:
                printf("Unregognized type: %ld, ", recv.msg_type);
                printf("ignoring message: %s\n", recv.msg_text);
        }
    }
}
