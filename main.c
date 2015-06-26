#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "ae.h"

aeFileProc *doWrite(struct aeEventLoop *eventLoop, int client_sk, void *clientData, int mask){
    if(clientData != NULL){
        send(client_sk, clientData, 1024, 0);
        clientData = NULL;
        free(clientData);
    }
    aeDeleteFileEvent(eventLoop, client_sk, AE_WRITABLE);
    /*close(client_sk);*/
}

aeFileProc *doRead(struct aeEventLoop *el, int client_sk, void *clientData, int mask){
    char *buff;
    int length;
    buff = (char *)malloc(sizeof(char) * 1024);
    memset(buff, sizeof(char), 1024);
    length = recv(client_sk, buff, 1024, 0);
    aeCreateFileEvent(el, client_sk, AE_WRITABLE, doWrite, buff);
}

aeFileProc *doAccept(struct aeEventLoop *el, int server_sk, void *clientData, int mask){
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    int client_sk = accept(server_sk, (struct sockaddr_in*)&client_addr, &length);
    aeCreateFileEvent(el, client_sk, AE_READABLE, doRead, clientData);
    if(client_sk < 0){
        perror("client");
    }
}

int main(int argc, char* argv[]){
    int server_sk = 0;
    void *clientData;
    clientData = NULL;
    
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(8000);

    if((server_sk = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
    }

    if(bind(server_sk, &local_addr, sizeof(struct sockaddr)) < 0){
        perror("bind");
    }
    if(listen(server_sk, 1) < 0){
        perror("listen");
    }

    aeEventLoop *el = aeCreateEventLoop(1024);
    aeCreateFileEvent(el, server_sk, AE_READABLE, doAccept, clientData);
    
    aeMain(el);
    close(server_sk);
    return -1;
}
