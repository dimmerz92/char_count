#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

int parse_args(int argc, char *argv[], int *nprocs, DIR **dir);
//int get_files();

int main(int argc, char *argv[]){
    DIR *dir;
    struct dirent *entry;
    int nprocs;

    if (parse_args(argc, argv, &nprocs, &dir)) {
        perror("Usage: char_count directory_name");
    }
    while ((entry = readdir(dir)) != NULL){
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
    return 0;
}

int parse_args(int argc, char *argv[], int *nprocs, DIR **dir){
    if (argc != 3) return -1;
    if ((*nprocs = atoi(argv[1])) < 1) return -2;
    if ((*dir = opendir(argv[2])) == NULL) return -3;
    return 0;
}
