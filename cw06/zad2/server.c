#include <stdio.h>
#include <fcntl.h>  
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "headers.h"

mqd_t clients[100];
int client_id = 0;

void server_exit(){
    printf("Finishing server process...\n");
    for(int i=0;i<client_id;++i)mq_close(clients[i]);
    mq_unlink(SERVERNAME);
    exit(0);
}

void close_client(char *message){
    int id, i = message[0];
    if(i<0)return;
    id = clients[i];
    printf("Closing client %d... ", i);
    if(mq_close(id) < 0)
        printf("%s\n", strerror(errno));
    else
        printf("OK\n");
}

void sigint(int signo){
    printf("Received SIGINT signal\n");
    server_exit();
}

int start_client(int id){
    char message[MSGTXTLEN];
    printf("Received client request\n");
    sprintf(message, "%d", client_id);
    if(mq_send(id, message, MSGTXTLEN, MSGPRIO) < 0){
        printf("Client not accepted: ");
        printf("%s\n", strerror(errno));
        return 0;
    }
    printf("Client accepted with id: %d\n",client_id);
    return 1;
}

void handle(char *message, char *cmd){
    int id, i = message[0];
    id = clients[i];
    printf("%d\n", i);

    FILE *f = popen(cmd, "r");
    fgets(message, 100, f);
    pclose(f);
    if(mq_send(id, message, MSGTXTLEN, MSGPRIO) < 0){
        printf("Not responded: ");
        printf("%s\n", strerror(errno));
    }
}

void handle_mirror(char* message){
    printf("Received MIRROR request from ");
    char cmd[80];
    sprintf(cmd, "echo %s | rev", message+1);
    handle(message, cmd);
}

void handle_time(char *message){
    printf("Received TIME request from: ");
    char cmd[80];
    sprintf(cmd, "date");
    handle(message, cmd);
}

void handle_calc(char *message){
    printf("Received CALC request from: ");
    char cmd[80];
    sprintf(cmd, "echo %s | bc", message+1);
    handle(message, cmd);
}

int main(){
    signal(SIGINT, &sigint);
    printf("Starting server...\n");
    printf("Creating messsage queue... ");
    mqd_t msg_id = mq_open(SERVERNAME, O_RDONLY | O_CREAT, MSGPERM, NULL);
    printf("OK\n");
    printf("Server initalized\n\n");
    char message[MSGTXTLEN];
    char *m = message;
    while(2){
        mq_receive(msg_id, message, MSGTXTLEN, NULL);
        switch(m[0]){
            case REQ_CLIENT_START:
                clients[client_id] = mq_open(message+1, O_WRONLY);
                if(start_client(clients[client_id]))client_id++;
                break;
            case REQ_MIRROR:
                handle_mirror(m+1);
                break;
            case REQ_TIME:
                handle_time(m+1);
                break;
            case REQ_CALC:
                handle_calc(m+1);
                break;
            case REQ_STOP:
                close_client(m+1);
                break;
            case REQ_END:
                printf("Received END request\n");
                server_exit();
                break;
            default:
                printf("Unregognized type: %d, ", message[0]);
                printf("ignoring message: %s\n", message+1);
        }
    }
}
