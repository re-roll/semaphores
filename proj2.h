#ifndef _PROJ2_H_
#define _PROJ2_H_

#define _GNU_SOURCE

//------------------Including libraries---------------------//
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

#define ERR_NO      0 //------------No errors---------------//
#define ERR_FORMAT  1 //---Input doesn't have right format--//
#define ERR_MEMORY  1 //-Problem with semaphores or memory--//

//---------------Structure for arguments--------------------//
typedef struct argsDef {
    int NO, NH, TI, TB;
} argsDef;

//------------------Protype declaration---------------------//
argsDef argsGet(int argc, char *argv[]);

void semInit();
int convToI(char *str);
void ffprintf(FILE *fp1, char const *fmt, ...);
int programExit(int errType);
void mainP(argsDef args);
void oxygenP(int i, argsDef args);
void hydrogenP(int i, argsDef args);
void createMol(char *HO, int i);
void mySleep(int msec);

#endif
