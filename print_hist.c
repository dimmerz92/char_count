#include "headers.h"

/* Prints a histogram for the counts */
void print_hist(int *counts) {
    int bar_height;

    for (int i = 0; i < 26; i++) {
        bar_height = counts[i]*500.0/counts[26];
        fprintf(stderr, "Process 1 got char %c: %d\t", 'a' + i, counts[i]);
        if (counts[i]) fprintf(stderr, "*");
        for (int j = 0; j < bar_height; j++) {
            fprintf(stderr, "*");
        }
        fprintf(stderr, "\n");
    }
}
