//Queue defines
#define MSGTXTLEN 128
#define MSGPERM 0600   

struct msg {
    long msg_type;
    char msg_text[MSGTXTLEN];
};

#define SERVER_ID 8

//Requests
#define REQ_CLIENT_START 8
#define REQ_MIRROR 1
#define REQ_TIME 2
#define REQ_CALC 3
#define REQ_END 4
#define SERVER_RESPONSE 8
