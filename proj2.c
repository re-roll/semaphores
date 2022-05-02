/* Author: Dmitrii Ivanushkin xivanu00 */

#include "proj2.h"

FILE *pf = NULL;   //--------File name---------//

sem_t *mutex,      //--Main mutable semaphore--//
      *barrier,    //-Barrier for new moleculs-//
      *oxyQueue,   //--Oxygen waits in queue---//
      *hydroQueue; //-Hydrogen waits in queue--//

int *oxygen,       //---Counter for oxygens----//
    *hydrogen;     //--Counter for hydrogens---//

int *action;       //----Counter for lines-----//

int main(int argc, char *argv[])
{
    argsDef args = argsGet(argc, argv); // Getting arguments
    
    // Open file
    if ((pf = fopen("proj2.out", "w")) == NULL){
        fprintf(stderr, "ERROR opening file\n");
        exit(ERR_FORMAT);
    }

    // Open all semaphores and memory
    semInit();

    // Call main process
    mainP(args);

    // Waiting (inactive)
    while(wait(NULL) > 0);

    return 0;
}

void mainP(argsDef args)
{
    for (int i = 0; i < args.NO; i++){ // From zero to number of oxygens
        pid_t pid = fork();            // Call fork
        if (pid == 0){
            oxygenP();                 // Call process "oxygen"
            exit(0);
        }
        else if (pid == -1){
            fprintf(stderr, "ERROR calling fork()\n");
            exit(ERR_MEMORY);
        }
    }

    for (int i = 0; i < args.NH; i++){ // From zero to number of hydrogens
        pid_t pid = fork();            // Call fork
        if (pid == 0){
            hydrogenP();               // Call process "hydrogen"
            exit(0);
        }
        else if (pid == -1){
            fprintf(stderr, "ERROR calling fork()\n");
            exit(ERR_MEMORY);
        }
    }

    exit(ERR_NO); // Call process to destroy semaphores, free memory and exit program
}

void oxygenP()
{
    (*oxygen)++; // Number of current atom

    sem_wait(mutex);
    ffprintf(pf, "%d: O: %d: started\n", *action, *oxygen);
    sem_post(barrier);
}

void hydrogenP()
{
    (*hydrogen)++; // Number of current atom

    sem_post(mutex);
    ffprintf(pf, "%d: H: %d: started\n", *action, *hydrogen);
    sem_wait(barrier);
}

void semInit()
{
    int err = ERR_NO;

    if ((mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED)
        err = ERR_MEMORY;
    if ((barrier = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED)
        err = ERR_MEMORY;
    if ((oxyQueue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED)
        err = ERR_MEMORY;
    if ((hydroQueue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED)
        err = ERR_MEMORY;
        
    if (err == ERR_MEMORY)
        exit(ERR_MEMORY);
    else{
        sem_init(mutex, 1, 1);
        sem_init(barrier, 1, 1);
        sem_init(oxyQueue, 1, 1);
        sem_init(hydroQueue, 1, 1);

        oxygen = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
        (*oxygen) = 0;
        hydrogen = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
        (*hydrogen) = 0;
        action = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
        (*action) = 1;
    }
}

void ffprintf(FILE *pf, char const *fmt, ...)
{
    va_list ap1, ap2;       // *
    va_start(ap1, fmt);     // Getting arguments of printed string
    va_copy(ap2, ap1);      // *

    (*action)++;            // Number of line incrementation

    vprintf(fmt, ap1);      // Write to console
    vfprintf(pf, fmt, ap2); // Write to file

    if (fflush(pf) == EOF){ // Buffer free
        fprintf(stderr, "ERROR Failed to free buffer");
        exit(ERR_MEMORY);
    }

    va_end(ap2);
    va_end(ap1);
}

int convToI(char *str) // Convert arguments from input of calling program to integers
{
    int val = atoi(str);

    return val;
}

argsDef argsGet(int argc, char *argv[])
{
    if (argc != 5) { // Checking if we have exactly 4 arguments after filename  
        fprintf(stderr, "ERROR missing arguments\n");
        exit(ERR_FORMAT);
    }

    argsDef args;               // Structure initialization

    // Convertation of strings to integers
    args.NO = convToI(argv[1]);
    args.NH = convToI(argv[2]);
    args.TI = convToI(argv[3]); 
    args.TB = convToI(argv[4]); 

    // Range format errors
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
    fclose(pf); // Close file

    // Destroying semaphores and getting possible errors
    if (sem_destroy(mutex) == -1) 
        errType = ERR_MEMORY;
    if (sem_destroy(barrier) == -1) 
        errType = ERR_MEMORY;
    if (sem_destroy(oxyQueue) == -1) 
        errType = ERR_MEMORY;
    if (sem_destroy(hydroQueue) == -1) 
        errType = ERR_MEMORY;

    // Free memory
    munmap(mutex, sizeof(sem_t));
    munmap(barrier, sizeof(sem_t));
    munmap(oxyQueue, sizeof(sem_t));
    munmap(hydroQueue, sizeof(sem_t));

    munmap(oxygen, sizeof(int));
    munmap(hydrogen, sizeof(int));
    munmap(action, sizeof(int));

    // Exit program with right type of error
    exit(errType);
}
