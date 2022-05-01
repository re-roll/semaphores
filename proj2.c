/* Author: Dmitrii Ivanushkin xivanu00 */

#include "proj2.h"

FILE *pf  = NULL;

sem_t *mutex,
      *barrier,
      *oxyQueue,
      *hydroQueue;

int *oxygen,
    *hydrogen;

int *action;

int main(int argc, char *argv[])
{
    argsDef args = argsGet(argc, argv);
    
    if ((pf = fopen("proj2.out", "w")) == NULL){
        fprintf(stderr, "ERROR opening file\n");
        exit(ERR_FORMAT);
    }

    mainP(args);

    while(wait(NULL) > 0);

    return 0;
}

void mainP(argsDef args)
{
    for (int i = 0; i < args.NO; i++){
        pid_t oxy = fork();
        if (oxy == 0){
            oxygenP(i);
            exit(0);
        }
        else if (oxy == -1){
            fprintf(stderr, "ERROR calling fork()\n");
            exit(ERR_MEMORY);
        }
    }

    for (int i = 0; i < args.NH; i++){
        pid_t hyd = fork();
        if (hyd == 0){
            hydrogenP(i);
            exit(0);
        }
        else if (hyd == -1){
            fprintf(stderr, "ERROR calling fork()\n");
            exit(ERR_MEMORY);
        }
    }

    exit(ERR_NO);
}

void oxygenP(int cnt)
{
    ffprintf(pf, "n: O: %d: started\n", cnt);
}

void hydrogenP(int cnt)
{
    ffprintf(pf, "n: H: %d: started\n", cnt);
}

void semInit()
{
    //int *oxygen = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    //int *hydrogen = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);

    mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    barrier = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    oxyQueue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    hydroQueue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    sem_init(mutex, 1, 1);
    sem_init(barrier, 1, 1);
    sem_init(oxyQueue, 1, 1);
    sem_init(hydroQueue, 1, 1);
}

void ffprintf(FILE *pf, char const *fmt, ...)
{
    va_list ap1, ap2;

    va_start(ap1, fmt);
    va_copy(ap2, ap1);

    vprintf(fmt, ap1);
    vfprintf(pf, fmt, ap2);

    va_end(ap2);
    va_end(ap1);

    if (fflush(pf) == EOF){
        fprintf(stderr, "Failed to buffer");
        exit(ERR_MEMORY);
    }

    return;
}

int convToI(char *str)
{
    int val;
    val = atoi(str);
    return val;
}

argsDef argsGet(int argc, char *argv[])
{
    if (argc != 5) {
        fprintf(stderr, "ERROR missing arguments\n");
        exit(ERR_FORMAT);
    }

    argsDef args;
    args.NO = convToI(argv[1]);
    args.NH = convToI(argv[2]);
    args.TI = convToI(argv[3]);
    args.TB = convToI(argv[4]);
    
    if (args.NO <= 0){
        fprintf(stderr, "ERROR Number of 'O' must be greater than 0\n");
        exit(ERR_FORMAT);
    }
    if (args.NH <= 0){
        fprintf(stderr, "ERROR Number of 'H' must be greater than 0\n");
        exit(ERR_FORMAT);
    }
    if ((args.TI < 0) || (args.TI > 1000)){
        fprintf(stderr, "ERROR Timeout for going to queue must be greater than 0 and lesser than 1000\n");
        exit(ERR_FORMAT);
    }
    if ((args.TB < 0) || (args.TB > 1000)){
        fprintf(stderr, "ERROR Timeout for creating molecula must be greater than 0 and lesser than 1000\n");
        exit(ERR_FORMAT);
    }

    return args;
}

int programExit(int errType)
{
    sem_destroy(mutex);
    sem_destroy(barrier);
    sem_destroy(oxyQueue);
    sem_destroy(hydroQueue);

    munmap(mutex, sizeof(sem_t));
    munmap(barrier, sizeof(sem_t));
    munmap(oxyQueue, sizeof(sem_t));
    munmap(hydroQueue, sizeof(sem_t));

    munmap(oxygen, sizeof(int));
    munmap(hydrogen, sizeof(int));

    exit(errType);
}
