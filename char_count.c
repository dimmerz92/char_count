#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

int parse_args(int argc, char *argv[], int *nprocs, DIR **dir);
void get_files(DIR *dir, char *files[]);

int main(int argc, char *argv[]) {
    DIR *dir;
    int nprocs;
    char **files = NULL;

    if (parse_args(argc, argv, &nprocs, &dir) < 0) {
        perror("Usage: char_count directory_name");
    }
    
    get_files(dir, files);

    /*for (int n = 0; n < 2; n++) {
        printf("%s\n", files[n]);
    }*/
    closedir(dir);
    free(files);
    return 0;
}

int parse_args(int argc, char *argv[], int *nprocs, DIR **dir) {
    if (argc != 3) return -1;
    if ((*nprocs = atoi(argv[1])) < 1) return -2;
    if ((*dir = opendir(argv[2])) == NULL) return -3;
    return 0;
}

void get_files(DIR *dir, char *files[]) {
    int i = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        files = realloc(files, (i+1) * sizeof(char*));
        files[i] = entry->d_name;
    }
}
