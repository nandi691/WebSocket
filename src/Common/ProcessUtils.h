#ifndef PROCESS_UTILS_H
#define PROCESS_UTILS_H
#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/prctl.h>

// Define the macro for process creation
#define PROCESS_CREATION(procName)                     \
    do {                                               \
        pid_t pid = fork();                            \
        if (pid == 0) {                                \
            /* Child process */                       \
			SetProcessName(procName);                 \
        } else if (pid > 0) {                          \
            /* Parent process */                        \
            return;                  					\
        } else {                                       \
            /* Fork failed */                          \
            perror("Fork failed");                     \
            exit(EXIT_FAILURE);\
        }                                              \
    } while (0)

void SetProcessName(const char* procName);
void SpawnOtherProcess();
void SpawnWebSocketServer();

#endif
