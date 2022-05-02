/* Author: Dmitrii Ivanushkin xivanu00 */

#include "proj2.h"

FILE *pf = NULL;   //--------File name---------//

sem_t *mutex,      //--Main mutable semaphore--//
      //*mutex2,   //-Mutable semaphore (help)-//
      //*barrier,  //-Barrier for new moleculs-//
      //*barrier2, //-Barrier for mlcls (help)-//
      *writeStr,   //----Wait until writing----//
      *oxyQueue,   //--Oxygen waits in queue---//
      *hydroQueue; //-Hydrogen waits in queue--//

int *oxygen,       //---Counter for oxygens----//
    *hydrogen;     //--Counter for hydrogens---//

int *action,       //----Counter for lines-----//
    *bondcnt;

int main(int argc, char *argv[])
{
    argsDef args = argsGet(argc, argv); // Getting arguments
    
    // Open file
    if ((pf = fopen("proj2.out", "w")) == NULL){
        fprintf(stderr, "ERROR opening file\n");
        exit(ERR_FORMAT);
    }

    semInit(); // Open all semaphores and memory

    mainP(args); // Call main process

    while(wait(NULL) > 0); // Waiting (inactive)

    return 0;
}

void mainP(argsDef args)
{
    for (int i = 1; i <= args.NO; i++){ // From zero to number of oxygens
        pid_t pid = fork();             // Call fork
        if (pid == 0){
            oxygenP(i, args);                 // Call process "oxygen"
            exit(0);
        }
        else if (pid == -1){
            fprintf(stderr, "ERROR calling fork()\n");
            programExit(ERR_MEMORY);
        }
    }

    for (int i = 1; i <= args.NH; i++){ // From zero to number of hydrogens
        pid_t pid = fork();             // Call fork
        if (pid == 0){
            hydrogenP(i, args);               // Call process "hydrogen"
            exit(0);
        }
        else if (pid == -1){
            fprintf(stderr, "ERROR calling fork()\n");
            programExit(ERR_MEMORY);
        }
    }

    programExit(ERR_NO); // Call process to destroy semaphores, free memory and exit program
}

void oxygenP(int i, argsDef args)
{
    //int count = 0;

    srand(time(NULL) * getpid());
    (*oxygen)++;

    sem_wait(mutex);

    sem_wait(writeStr);
    ffprintf(pf, "%d: O %d: started\n", *action, i);
    sem_post(writeStr);

    if (*hydrogen >= 2){
        (*bondcnt)++;
        
        sem_post(hydroQueue);
        sem_post(hydroQueue);

        (*hydrogen) -= 2;

        sem_post(oxyQueue);

        (*oxygen)--;
    }
    else{
        sem_post(mutex);
    }

    sem_wait(oxyQueue);

    mySleep(args.TI);

    sem_wait(writeStr);
    ffprintf(pf, "%d: O %d: going to queue\n", *action, i);
    sem_post(writeStr);
    
    createMol("O", i);

    mySleep(args.TB);

    /*sem_wait(mutex2);
    count++;
    if (count == 3){
        sem_wait(barrier2);
        sem_post(barrier);
    }
    sem_post(mutex2);

    sem_wait(barrier);
    sem_post(barrier);

    sem_wait(writeStr);
    ffprintf(pf, "%d: O %d: molecule %d created\n", *action, i, count);
    sem_post(writeStr);
    
    sem_wait(mutex2);
    count--;
    if (count == 0){
        sem_wait(barrier);
        sem_post(barrier2);
    }
    sem_post(mutex2);

    sem_wait(barrier2);
    sem_post(barrier2);*/

    sem_wait(writeStr);
    ffprintf(pf, "%d: O %d: molecule %d created\n", *action, i, *bondcnt);
    sem_post(writeStr);

    sem_post(mutex);

    if ((*hydrogen == 1) && (i == args.NO)){
        sem_wait(writeStr);
        ffprintf(pf, "%d: O %d: not enough H\n", *action, i);
        sem_post(writeStr);
    }

    
}

void hydrogenP(int i, argsDef args)
{
    //int count = 0;

    srand(time(NULL) * getpid());
    (*hydrogen)++;

    sem_wait(mutex);

    sem_wait(writeStr);
    ffprintf(pf, "%d: H %d: started\n", *action, i);
    sem_post(writeStr);

    if ((*hydrogen >= 2) && (*oxygen >= 1)){
        (*bondcnt)++;
        sem_post(hydroQueue);
        sem_post(hydroQueue);
    
        (*hydrogen) -= 2;

        sem_post(oxyQueue);

        (*oxygen)--;
    }
    else{
        sem_post(mutex);
    }

    sem_wait(hydroQueue);

    mySleep(args.TI);

    sem_wait(writeStr);
    ffprintf(pf, "%d: H %d: going to queue\n", *action, i);
    sem_post(writeStr);

    createMol("H", i);

    mySleep(args.TB);

    /*sem_wait(mutex2);
    count++;
    if (count == 3){
        sem_wait(barrier2);
        sem_post(barrier);
    }
    sem_post(mutex2);

    sem_wait(barrier);
    sem_post(barrier);

    sem_wait(writeStr);
    ffprintf(pf, "%d: H %d: molecule %d created\n", *action, i, count);
    sem_post(writeStr);
    
    sem_wait(mutex2);
    count--;
    if (count == 0){
        sem_wait(barrier);
        sem_post(barrier2);
    }
    sem_post(mutex2);

    sem_wait(barrier2);
    sem_post(barrier2);*/

    sem_wait(writeStr);
    ffprintf(pf, "%d: H %d: molecule %d created\n", *action, i, *bondcnt);
    sem_post(writeStr);

    sem_post(mutex);

    if (((*hydrogen == 1) || (*oxygen == 0)) && (i == args.NO)){
        sem_wait(writeStr);
        ffprintf(pf, "%d: H %d: not enough O or H\n", *action, i);
        sem_post(writeStr);
    }
}

void createMol(char *HO, int i)
{
    sem_wait(writeStr);
    ffprintf(pf, "%d: %s %d: creating molecule %d\n", *action, HO, i, *bondcnt);
    sem_post(writeStr);
}

void mySleep(int msec)
{
    usleep(1000 * (rand() % (msec + 1)));
}

void semInit()
{
    int err = ERR_NO;

    if ((mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED)
        err = ERR_MEMORY;
    //if ((mutex2 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED)
    //   err = ERR_MEMORY;
    //if ((barrier = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED)
    //   err = ERR_MEMORY;
    //if ((barrier2 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED)
    //   err = ERR_MEMORY;
    if ((writeStr = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED)
        err = ERR_MEMORY;
    if ((oxyQueue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED)
        err = ERR_MEMORY;
    if ((hydroQueue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED)
        err = ERR_MEMORY;
        
    if (err == ERR_MEMORY)
        exit(ERR_MEMORY);
    
    sem_init(mutex, 1, 1);
    //sem_init(mutex2, 1, 1);
    //sem_init(barrier, 1, 0);
    //sem_init(barrier2, 1, 1);
    sem_init(writeStr, 1, 1);
    sem_init(oxyQueue, 1, 0);
    sem_init(hydroQueue, 1, 0);

    oxygen = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    (*oxygen) = 0;
    hydrogen = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    (*hydrogen) = 0;
    action = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    (*action) = 1;
    bondcnt = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    (*bondcnt) = 0;
    
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

argsDef argsGet(int argc, char *argv[])
{
    // Checking if we have exactly 4 arguments after filename
    if (argc != 5) {  
        fprintf(stderr, "ERROR missing arguments\n");
        exit(ERR_FORMAT);
    }

    argsDef args; // Structure initialization             

    // Convertation of strings to integers
    args.NO = atoi(argv[1]);
    args.NH = atoi(argv[2]);
    args.TI = atoi(argv[3]); 
    args.TB = atoi(argv[4]); 

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
    /*if (sem_destroy(mutex2) == -1) 
        errType = ERR_MEMORY;
    if (sem_destroy(barrier) == -1) 
        errType = ERR_MEMORY;
    if (sem_destroy(barrier2) == -1) 
        errType = ERR_MEMORY;*/
    if (sem_destroy(writeStr) == -1) 
        errType = ERR_MEMORY;
    if (sem_destroy(oxyQueue) == -1) 
        errType = ERR_MEMORY;
    if (sem_destroy(hydroQueue) == -1) 
        errType = ERR_MEMORY;

    // Free memory
    munmap(mutex, sizeof(sem_t));
    //munmap(mutex2, sizeof(sem_t));
    //munmap(barrier, sizeof(sem_t));
    //munmap(barrier2, sizeof(sem_t));
    munmap(writeStr, sizeof(sem_t));
    munmap(oxyQueue, sizeof(sem_t));
    munmap(hydroQueue, sizeof(sem_t));

    munmap(oxygen, sizeof(int));
    munmap(hydrogen, sizeof(int));
    munmap(action, sizeof(int));
    munmap(bondcnt, sizeof(int));

    exit(errType); // Exit program with right type of error
}
