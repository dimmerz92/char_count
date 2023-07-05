#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

int parse_args(int argc, char *argv[], int *nprocs, DIR **dir);
int get_files(DIR *dir, char **files);
int init_ring();
int add_node(int *pid);

int main(int argc, char *argv[]) {
    DIR *dir;
    int nprocs;
    char **files;

    /* parses arguments and sets initial variables */
    if (parse_args(argc, argv, &nprocs, &dir) < 0) {
        fprintf(stderr, "Usage: char_count directory_name\n");
        return -1;
    }

    /* allocates memory to the array of filenames */
    if ((files = malloc(sizeof(char *))) == NULL) {
        fprintf(stderr, "Could not allocate memory for files array\n");
        return -1;
    }
    
    /* gets filenames "*.txt" and stores in array*/
    if (get_files(dir, files) < 0) {
        fprintf(stderr, "Error getting files\n");
        free(files);
        return -1;
    }

    closedir(dir);
    free(files);
    return 0;
}

/* checks arg numbers, assigns nprocs and dir from command args */
int parse_args(int argc, char *argv[], int *nprocs, DIR **dir) {
    if (argc != 3) return -1;
    if ((*nprocs = atoi(argv[1])) < 1) return -2;
    if ((*dir = opendir(argv[2])) == NULL) return -3;
    return 0;
}

/* uses regex to get all .txt files from dir arg and assigns to files arg */
int get_files(DIR *dir, char **files) {
    int i = 0;
    regex_t regex;
    struct dirent *entry;

    regcomp(&regex, "\\.txt$", 0);

    while ((entry = readdir(dir)) != NULL) {
        if (regexec(&regex, entry->d_name, 0, NULL, 0) != 0) continue;
        if ((files = realloc(files, (i + 1) * sizeof(char *))) == NULL) {
            return -1;
        }
        files[i] = entry->d_name;
        i++;
    }

    if (i == 0) {
        return -1;
    }
    return 0;
}

int init_ring() {
    int fd[2];

    if (pipe(fd) < 0) {
        return -1;
    }
    if (dup2(fd[0], STDIN_FILENO) < 0 || dup2(fd[1], STDOUT_FILENO) < 0) {
        return -2;
    }
    if (close(fd[0]) < 0 || close(fd[1]) < 0) {
        return -3;
    }
    return 0;
}

int add_node(int *pid) {
    int fd[2];

    if (pipe(fd) < 0) {
        return -1;
    }
    if ((*pid = fork()) < 0) {
        return -2;
    }
    if (*pid > 0 && dup2(fd[1], STDOUT_FILENO) < 0) {
        return -3;
    }
    if (*pid == 0 && dup2(fd[0], STDIN_FILENO) < 0) {
        return -4;
    }
    if (close(fd[0]) < 0 || close(fd[1]) < 0) {
        return -5;
    }
    return 0;
}
