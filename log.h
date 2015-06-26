#ifndef __LOG_H__
#define __LOG_H__

#define LOG_INFO_STR  "[INFO] %s\r\n"
#define LOG_WARNING_STR "[WARNING] %s\r\n" 
#define LOG_ERROR_STR "[ERROR] %s\r\n"

void info(char* msg);
void warning(char* msg);
void error(char* msg);

#endif
