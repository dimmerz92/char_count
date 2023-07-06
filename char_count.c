#include <stdio.h>
#include <fcntl.h>
#include <regex.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

/* Check OS for correct filepath separator */
#ifdef _WIN32
const char SEP[] = "\\";
#else
const char SEP[] = "/";
#endif

/* struct to hold character counts */
typedef struct {
    int counts[26];
    int total;
} char_counts;

/* Pre-declared functions */
int parse_args(int argc, char *argv[], int *nprocs, char **d_path, DIR **dir);
int get_files(DIR *dir, char **files);
int init_ring(void);
int add_node(int *pid);
char_counts get_counts(char *path);

/* MAIN */
int main(int argc, char *argv[]) {
    int i;
    int pid;
    int nprocs;
    char *d_path;
    DIR *dir;
    
    /* parses arguments and sets initial variables */
    if (parse_args(argc, argv, &nprocs, &d_path, &dir) < 0) {
        fprintf(stderr, "Usage: char_count directory_name\n");
        return -1;
    }
    
    /* gets filenames "*.txt" and stores in files array*/
    char *files[nprocs];
    if (get_files(dir, files) < 0) {
        fprintf(stderr, "Error getting files\n");
        return -1;
    }

    /* Inits ring structure */
    if (init_ring() < 0) {
        fprintf(stderr, "Error initialising ring structure\n");
    }

    /* Adds nodes to the ring */
    for (i = 1; i < nprocs; i++) {
        if (add_node(&pid) < 0) {
            fprintf(stderr, "Error adding node\n");
            return -1;
        }
        if (pid) break;
    }

    /* Counts letters in file */

    
    return 0;
}

/* checks arg numbers, assigns nprocs and dir from command args */
int parse_args(int argc, char *argv[], int *nprocs, char **d_path, DIR **dir) {
    if (argc != 3) return -1;
    if ((*nprocs = atoi(argv[1])) < 1) return -2;
    if ((*dir = opendir(argv[2])) == NULL) return -3;
    if ((d_path = malloc((strlen(argv[2]) + 1) * sizeof(char))) == NULL) {
        return -4;
    }
    strcpy(*d_path, argv[2]);
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
        files[i] = entry->d_name;
        i++;
    }

    closedir(dir);

    if (i == 0) {
        return -1;
    }
    return 0;
}

int init_ring(void) {
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

char_counts get_counts(char *path) {
    char *path_to_file;
    size_t path_len, file_len;

    if (path[strlen(path)] == '/' || path[strlen(path)] == '\\') {
        path_len = strlen(path);
        file_len = strlen(path);
    }
    int testfile = open("text_files/98-0.txt", O_RDONLY);
    char buf[21];
    read(testfile, buf, 20);
    printf("%s\n", buf);

    close(testfile);
}
