#include <stdio.h>
#include "log.h"

FILE *logFd = NULL;

static void logFile(char* msg, char* format){
    char *str = NULL;
    int len = 0;
    if(logFd == NULL){
        logFd = fopen("./log", "a");
        if(logFd == NULL){
            perror("error in log");
        }
    }
    fprintf(logFd, format, msg);
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
