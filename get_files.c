#include "headers.h"

/* uses regex to get all .txt files from dir arg and assigns to files arg */
int get_files(DIR *dir, char ***files, int *nfiles, int nprocs) {
    *nfiles = 0;
    regex_t regex;
    struct dirent *entry;

    // use regular expression to check if file is .txt
    regcomp(&regex, "\\.txt$", 0);

    // get name of all .txt files and add to array
    while ((entry = readdir(dir)) != NULL) {
        if (regexec(&regex, entry->d_name, 0, NULL, 0) != 0) continue;
        *files = realloc(*files, (*nfiles + 1) * sizeof(char *));
        (*files)[*nfiles] = malloc(strlen(entry->d_name) * sizeof(char) + 1);
        strcpy((*files)[*nfiles], entry->d_name);
        *nfiles+=1;
    }

    closedir(dir);
    regfree(&regex);

    if (*nfiles == 0) {
        fprintf(stderr, "Error no txt files in directory\n");
        return -1;
    } else if (nprocs > *nfiles) {
        fprintf(stderr, "Error user specified more processes than txt files\n");
        return -2;
    }
    return 0;
}
