#include <stdio.h>
#include "log.h"


static void logFile(char* msg, char* format){
    FILE *logFd;
    char *str = NULL;
    int len = 0;
    logFd = fopen("./log", "a");
    if(logFd == NULL){
        perror("error in log");
    }
    fprintf(logFd, format, msg);
    fclose(logFd);
}

void info(char* msg){
    logFile(msg, LOG_INFO_STR);
}

void warning(char* msg){
    logFile(msg, LOG_WARNING_STR);
}

void error(char* msg){
    logFile(msg, LOG_ERROR_STR);
}
