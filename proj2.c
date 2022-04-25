/*
  ***********************************
  Author: Dmitrii Ivanushkin xivanu00
  ***********************************
*/

#include "proj2.h"

argsDef args;
FILE *pf = NULL;

int main(int argc, char *argv[])
{
    errno = 0;

    args = handleArgs(argc, argv);

    pf = fopen("proj2.out", "w");

    if (pf == NULL){
        handleErrs(ERR_FORM, ERR_EXIT, errno, "ERROR opening file\n");
    }

    programExit(ERR_NONE);
}

int convToI(char *str)
{
    int val;
    val = atoi(str);
    return val;
}

argsDef handleArgs(int argc, char *argv[])
{
    errno = 0;

    if (argc < 5) {
        handleErrs(ERR_FORM, ERR_EXIT, errno, "ERROR missing arguments\n");
    }

    argsDef args;

    args.NO = convToI(argv[1]);
    args.NH = convToI(argv[2]);
    args.TI = convToI(argv[3]);
    args.TB = convToI(argv[4]);
    
    if (args.NO <= 0){
        handleErrs(ERR_FORM, ERR_EXIT, errno, "ERROR Number of 'O' must be greater than 0\n");
    }
    if (args.NH <= 0){
        handleErrs(ERR_FORM, ERR_EXIT, errno, "ERROR Number of 'H' must be greater than 0\n");
    }
    if ((args.TI < 0) || (args.TI > 1000)){
        handleErrs(ERR_FORM, ERR_EXIT, errno, "ERROR Timeout for going to queue must be greater than 0 and lesser than 1000\n");
    }
    if ((args.TB < 0) || (args.TB > 1000)){
        handleErrs(ERR_FORM, ERR_EXIT, errno, "ERROR Timeout for creating molecula must be greater than 0 and lesser than 1000\n");
    }

    return args;
}

void handleErrs(int ERR, bool ifEXIT, int errNum, char *strErr, ...)
{
    va_list args;
    va_start(args, strErr);

    vfprintf(stderr, strErr, args);

    va_end(args);
    
    if (errNum != 0)
        fprintf(stderr, "%s\n", strerror(errNum));
    
    if (ifEXIT == true)
        programExit(ERR);
}

int programExit(int errType)
{
    if ((pf != NULL) && (fclose(pf) == EOF)) {
        handleErrs(ERR_FORM, 0, errno, "Program cannot close file 'proj2.out'\n");
    }

    exit(errType);
}
