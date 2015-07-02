#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "zmalloc.h"
#include "ae.h"
#include "log.h"

aeFileProc *doWrite(struct aeEventLoop *eventLoop, int client_sk, void *clientData, int mask){
    if(clientData != NULL){
        char c[16] = {0};
        int len = 0;
        send(client_sk, clientData, strlen(clientData) + 1, 0);
        send(client_sk, c, strlen(c), 0);
        clientData = NULL;
        zfree(clientData);
    }
    aeDeleteFileEvent(eventLoop, client_sk, AE_WRITABLE);
}

aeFileProc *doRead(struct aeEventLoop *el, int client_sk, void *clientData, int mask){
    char *buff;
    int length;
    buff = (char *)zmalloc(sizeof(char) * 1024);
    memset(buff, 0, 1024);
    length = recv(client_sk, buff, 1024, 0);
    aeCreateFileEvent(el, client_sk, AE_WRITABLE, doWrite, buff);
}

aeFileProc *doAccept(struct aeEventLoop *el, int server_sk, void *clientData, int mask){
    info("accept");
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    int client_sk = accept(server_sk, (struct sockaddr_in*)&client_addr, &length);
    if(client_sk < 0){
        perror("client");
    }
    if(client_sk > 0){
        aeCreateFileEvent(el, client_sk, AE_READABLE, doRead, clientData);
    }
}

void forkWorker(aeEventLoop *el, int server_sk){
    void *clientData;
    clientData = NULL;
    int pid = fork();
    if(pid < 0){
        perror("fork");
    }
    else if(pid == 0){
        info("fork");
        aeCreateFileEvent(el, server_sk, AE_READABLE, doAccept, clientData);
        aeMain(el);
    }
}

int main(int argc, char* argv[]){
    int i = 1;
    aeEventLoop *el = aeCreateEventLoop(1024);

    int server_sk = 0;

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(8000);

    if((server_sk = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
    }

    if(bind(server_sk, &local_addr, sizeof(struct sockaddr_in)) < 0){
        perror("bind");
    }

    if(listen(server_sk, 1) < 0){
        perror("listen");
    }

    /*aeCreateFileEvent(el, server_sk, AE_READABLE, doAccept, NULL);*/
    forkWorker(el, server_sk);
    forkWorker(el, server_sk);
    forkWorker(el, server_sk);
    forkWorker(el, server_sk);

    pid_t pid = getpid();

    do{
        sleep(1);
    }while(waitpid(pid, NULL, WNOHANG) == 0);    


    return -1;
}
