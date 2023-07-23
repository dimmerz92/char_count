/**
 * Author:      Andrew Weymes
 * Purpose:     Counts all alphabet characters casted to lower case in all .txt
 *              files in a specified directory
 * How to use:
 *  CLI Usage:  ./char_count arg1 arg2
 *  CLI arg1:   an integer reflecting the number of processes to spawn
 *  CLI arg2:   a string containing the filepath to the directory containing all
 *              .txt files to be counted
 * 
 * On Success:  Successful running will result in a return of 0 and text based
 *              histogram of the sum of all individual alphabet characters in
 *              the .txt files
 * 
 * On Failure:  Returns a negative integer and a brief description of the issue
 *              encountered
 **/
#include "headers.h"

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
    int counts[COUNTER_SIZE] = {0};
    int files_per_proc = nfiles / nprocs;
    int extra_files = nfiles % nprocs;
    int start = i * files_per_proc + (i + 1 <= extra_files ? i : extra_files);
    int end = start + files_per_proc + (i + 1 <= extra_files ? 1 : 0);

    for (int j=start; j<end; j++) {
        get_counts(dir_path, files[j], counts);
    }

    /* Free files variables */
    for (int f = 0; f < nfiles; f++) {
        free(files[f]);
    }
    free(files);

    /* Send counts along the ring & print results*/
    int received_counts[COUNTER_SIZE];
    if (i) { // if not the original parent process
        // get results from previous process
        if (read(STDIN_FILENO, received_counts, sizeof(received_counts)) < 0) {
            fprintf(stderr, "Error receiving results\n");
            return -1;
        }
        // sum parent counts to current counts
        sum_counts(counts, received_counts);
        // send summed counts to next process
        if (write(STDOUT_FILENO, counts, sizeof(counts)) < 0) {
            fprintf(stderr, "Error sending results\n");
            return -1;
        }
    } else { // original parent entry
        // send counts to next process
        if (write(STDOUT_FILENO, counts, sizeof(counts)) < 0) {
            fprintf(stderr, "Error sending results\n");
            return -1;
        }
        // receive final total from last process
        if (read(STDIN_FILENO, received_counts, sizeof(received_counts)) < 0) {
            fprintf(stderr, "Error receiving results\n");
            return -1;
        }
        // print the sums and histogram
        print_hist(received_counts);
    }
    
    return 0;
}
