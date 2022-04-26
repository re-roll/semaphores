/* Author: Dmitrii Ivanushkin xivanu00 */

#include "proj2.h"

FILE *pf  = NULL;
sh_o *mem = NULL;

int main(int argc, char *argv[])
{
    int pid;

    argsDef args = argsGet(argc, argv);

    pf = fopen("proj2.out", "w");

    mem = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED,-1,0);
    if (mem == MAP_FAILED){
        fprintf(stderr, "ERROR mapping shared mempry object\n");
        exit(ERR_ONE);
    }

    if (pf == NULL){
        fprintf(stderr, "ERROR opening file\n");
        exit(ERR_ONE);
    }

    pid = fork();
    if (pid == 0) {
        vodikProces();
        exit(0);
    } else if (pid == -1) {
        fprintf(stderr, "ERROR cannot call fork()");
        exit(ERR_ONE);
    } else {
        pid_t vodikID = pid;
    }

    pid = fork();
    if (pid == 0) {
        hlavniProces();
        exit(0);
    } else if (pid == -1) {
        fprintf(stderr, "ERROR cannot call fork()");
        exit(ERR_ONE);
    } else {
        pid_t hlavniID = pid;
    }

    programExit(ERR_NONE);
}

void hlavniProces()
{

}

void kyslikProces()
{

}

void vodikProces()
{

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
        exit(ERR_ONE);
    }

    argsDef args;
    args.NO = convToI(argv[1]);
    args.NH = convToI(argv[2]);
    args.TI = convToI(argv[3]);
    args.TB = convToI(argv[4]);
    
    if (args.NO <= 0){
        fprintf(stderr, "ERROR Number of 'O' must be greater than 0\n");
        exit(ERR_ONE);
    }
    if (args.NH <= 0){
        fprintf(stderr, "ERROR Number of 'H' must be greater than 0\n");
        exit(ERR_ONE);
    }
    if ((args.TI < 0) || (args.TI > 1000)){
        fprintf(stderr, "ERROR Timeout for going to queue must be greater than 0 and lesser than 1000\n");
        exit(ERR_ONE);
    }
    if ((args.TB < 0) || (args.TB > 1000)){
        fprintf(stderr, "ERROR Timeout for creating molecula must be greater than 0 and lesser than 1000\n");
        exit(ERR_ONE);
    }

    return args;
}
int programExit(int errType)
{
    exit(errType);
}
