#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
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

/* Pre-declared functions */
int parse_args(int argc, char *argv[], int *nprocs, char *dir_path, DIR **dir);
int get_files(DIR *dir, char ***files, int *nfiles, int nprocs);
int init_ring(void);
int add_node(int *pid);
int get_counts(char *path, char *file, int *counts);
void sum_counts(int *counts, int *received_counts);
void print_hist(int *counts);

/* MAIN */
int main(int argc, char *argv[]) {
    int i;
    int pid;
    int nprocs;
    int nfiles;
    char dir_path[strlen(argv[2])+1];
    DIR *dir;
    
    /* parses arguments and sets initial variables */
    if (parse_args(argc, argv, &nprocs, dir_path, &dir) < 0) {
        fprintf(stderr, "Usage: char_count directory_name\n");
        return -1;
    }
    
    /* gets filenames "*.txt" and stores in files array*/
    char **files = NULL;
    if (get_files(dir, &files, &nfiles, nprocs) < 0) {
        return -1;
    }

    /* Inits ring structure */
    if (init_ring() < 0) {
        fprintf(stderr, "Error initialising ring structure\n");
    }

    /* Adds nodes to the ring */
    for (i = 0; i < nprocs - 1; i++) {
        if (add_node(&pid) < 0) {
            fprintf(stderr, "Error adding node\n");
            return -1;
        }
        if (pid) break;
    }

    /* Counts letters in allocated file(s) */
    int counts[27] = {0};
    int files_per_proc = nfiles / nprocs;
    int extra_files = nfiles % nprocs;
    int start = i * files_per_proc + (i + 1 <= extra_files ? i : extra_files);
    int end = start + files_per_proc + (i + 1 <= extra_files ? 1 : 0);

    for (int j=start; j<end; j++) {
        get_counts(dir_path, files[j], counts);
    }

    for (int f = 0; f < nfiles; f++) {
        free(files[f]);
    }
    free(files);

    /* Send counts along the ring & print results*/
    int received_counts[27];
    if (i) {
        if (read(STDIN_FILENO, received_counts, sizeof(received_counts)) < 0) {
            fprintf(stderr, "Error receiving results\n");
            return -1;
        }
        sum_counts(counts, received_counts);
        if (write(STDOUT_FILENO, counts, sizeof(counts)) < 0) {
            fprintf(stderr, "Error sending results\n");
            return -1;
        }
    } else {
        if (write(STDOUT_FILENO, counts, sizeof(counts)) < 0) {
            fprintf(stderr, "Error sending results\n");
            return -1;
        }
        if (read(STDIN_FILENO, received_counts, sizeof(received_counts)) < 0) {
            fprintf(stderr, "Error receiving results\n");
            return -1;
        }
        print_hist(received_counts);
    }
    
    return 0;
}

/* checks arg numbers, assigns nprocs and dir from command args */
int parse_args(int argc, char *argv[], int *nprocs, char *dir_path, DIR **dir) {
    if (argc != 3) return -1;
    if ((*nprocs = atoi(argv[1])) < 1) return -2;
    strcpy(dir_path, argv[2]);
    if ((*dir = opendir(argv[2])) == NULL) return -4;
    
    return 0;
}

/* uses regex to get all .txt files from dir arg and assigns to files arg */
int get_files(DIR *dir, char ***files, int *nfiles, int nprocs) {
    *nfiles = 0;
    regex_t regex;
    struct dirent *entry;

    regcomp(&regex, "\\.txt$", 0);

    while ((entry = readdir(dir)) != NULL) {
        if (regexec(&regex, entry->d_name, 0, NULL, 0) != 0) continue;
        *files = realloc(*files, (*nfiles + 1) * sizeof(char *));
        (*files)[*nfiles] = malloc(strlen(entry->d_name) * sizeof(char) + 1);
        strcpy((*files)[*nfiles], entry->d_name);
        *nfiles+=1;
    }

    closedir(dir);

    if (*nfiles == 0) {
        fprintf(stderr, "Error no txt files in directory\n");
        return -1;
    } else if (nprocs > *nfiles) {
        fprintf(stderr, "Error user specified more processes than txt files\n");
        return -2;
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

int get_counts(char *path, char *filename, int *counts) {
    char c;
    FILE *file;
    size_t path_len = strlen(path);
    size_t file_len = strlen(filename);
    char *path_to_file = malloc((path_len + file_len + 2) * sizeof(char));

    if (path[strlen(path)] == '/' || path[strlen(path)] == '\\') {
        strcpy(path_to_file, path);
        strcat(path_to_file, filename);
    } else {
        strcpy(path_to_file, path);
        strcat(path_to_file, SEP);
        strcat(path_to_file, filename);
    }

    if ((file = fopen(path_to_file, "r")) == NULL) return -1;

    while ((c = fgetc(file))) {
        if (c == EOF) {
            break;
        } else if (tolower(c) - 'a' >= 0 && tolower(c) - 'a' < 26) {
            counts[tolower(c) - 'a']++;
            counts[26]++;
        }
    }
    free(path_to_file);
    fclose(file);
    return 0;
}

void sum_counts(int *counts, int *received_counts) {
    for (int i = 0; i < 27; i++) {
        counts[i] += received_counts[i];
    }
}

void print_hist(int *counts) {
    int bar_height;

    for (int i = 0; i < 26; i++) {
        bar_height = counts[i]*500.0/counts[26];
        fprintf(stderr, "Process 1 got char %c: %d\t", 'a' + i, counts[i]);
        for (int j = 0; j < bar_height; j++) {
            fprintf(stderr, "*");
        }
        fprintf(stderr, "\n");
    }
}
