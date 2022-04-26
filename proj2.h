#ifndef _PROJ2_H_
#define _PROJ2_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/mman.h>

#define ERR_NONE 0 
#define ERR_ONE  1 

typedef struct argsDef {
    int NO, NH, TI, TB;
} argsDef;

typedef struct {
	int action, creating, finished;
}sh_o;

argsDef argsGet(int argc, char *argv[]);
int programExit(int errType);
int convToI(char *str);
void hlavniProces();
void kyslikProces();
void vodikProces();

#endif
