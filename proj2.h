#ifndef _PROJ2_H_
#define _PROJ2_H_

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/types.h>

#define ERR_NO      0 
#define ERR_FORMAT  1 
#define ERR_MEMORY  1 

typedef struct argsDef {
    int NO, NH, TI, TB;
} argsDef;

argsDef argsGet(int argc, char *argv[]);

void semInit();
int convToI(char *str);
void ffprintf(FILE *fp1, char const *fmt, ...);
int programExit(int errType);
void mainP();
void oxygenP(int cnt);
void hydrogenP(int cnt);

#endif
