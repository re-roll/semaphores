#ifndef _PROJ2_H_
#define _PROJ2_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <stdbool.h>

#define ERR_NONE 0 // No errors
#define ERR_FORM 1 // Invalid format
#define ERR_MEMO 2 // Semaphore or memory error

#define ERR_NOEX 0 // Handle without exit
#define ERR_EXIT 1 // Handle and exit

typedef struct argsDef {
    int NO, NH, TI, TB;
} argsDef;

argsDef handleArgs(int argc, char *argv[]);

void handleErrs(int ERR, bool ifEXIT, int errNum, char *strErr, ...);

#endif
