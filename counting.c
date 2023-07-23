#include "headers.h"

/* Counts all alphabet characters casted to lowercase in a .txt file */
int get_counts(char *path, char *filename, int *counts) {
    char c;
    FILE *file;
    size_t path_len = strlen(path);
    size_t file_len = strlen(filename);
    char *path_to_file = malloc((path_len + file_len + 2) * sizeof(char));

    // adds the correct file path separator depending on OS (Win/Linux)
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

/* Adds the received counts array to the current counts array */
void sum_counts(int *counts, int *received_counts) {
    for (int i = 0; i < 27; i++) {
        counts[i] += received_counts[i];
    }
}
