#include "headers.h"

/* checks arg numbers, assigns nprocs and dir from command args */
int parse_args(int argc, char *argv[], int *nprocs, char *dir_path, DIR **dir) {
    if (argc != 3) return -1;
    if ((*nprocs = atoi(argv[1])) < 1) return -2;
    strcpy(dir_path, argv[2]);
    if ((*dir = opendir(argv[2])) == NULL) return -4;
    
    return 0;
}
