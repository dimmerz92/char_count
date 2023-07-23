#ifndef MAIN_HEADER_FILE
#define MAIN_HEADER_FILE

/* Check OS for correct filepath separator */
#ifdef _WIN32
#define SEP "\\"
#else
#define SEP "/"
#endif

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#define COUNTER_SIZE 27 //array for n alphabet + counter

int parse_args(int argc, char *argv[], int *nprocs, char *dir_path, DIR **dir);
int get_files(DIR *dir, char ***files, int *nfiles, int nprocs);
int init_ring(void);
int add_node(int *pid);
int get_counts(char *path, char *file, int *counts);
void sum_counts(int *counts, int *received_counts);
void print_hist(int *counts);

#endif
