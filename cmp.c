#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

int compare_files(FILE *pfile1, FILE *pfile2, int ignore_case, int verbose)
{
    char buffer_file1[BUFFER_SIZE], buffer_file2[BUFFER_SIZE];
    int num_of_line = 1;

    // start reading the text files line by line
    while (fgets(buffer_file1, BUFFER_SIZE, pfile1) != NULL && fgets(buffer_file2, BUFFER_SIZE, pfile2) != NULL)
    {
        if (ignore_case)
        { // ignore caps lock
            for (int i = 0; buffer_file1[i]; i++)
            {
                buffer_file1[i] = tolower(buffer_file1[i]);
            }
            for (int i = 0; buffer_file2[i]; i++)
            {
                buffer_file2[i] = tolower(buffer_file2[i]);
            }
        }
        if (strcmp(buffer_file1, buffer_file2) != 0)
        { // check if file content is the same
            if (verbose)
            {
                printf("distinct\n");
            }
            return 1;
        }
        num_of_line++;
    }

    // check if there is still more content to read in one of the files, means they are not equal in size
    if (fgets(buffer_file1, BUFFER_SIZE, pfile1) != NULL || fgets(buffer_file2, BUFFER_SIZE, pfile2) != NULL)
    {
        if (verbose)
        {
            printf("distinct\n");
        }
        return 1;
    }
    if (verbose)
        printf("equal\n");
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    { // check that parameters are not missing
        fprintf(stderr, "usage: ./cmp <file1> <file2> [-v] [-i]\n");
        return 1;
    }
    int verbose = 0;
    int ignore_case = 0;
    int i = 3;
    if (argc > 3)
    {
        while (i < argc && argv[i][0] == '-')
        {
            if (strcmp(argv[i], "-v") == 0)
            { // check if user entered -v
                verbose = 1;
            }
            else if (strcmp(argv[i], "-i") == 0)
            { // check if user entered -i
                ignore_case = 1;
            }
            else
            {
                fprintf(stderr, "Invalid option: %s\n", argv[i]);
                exit(1);
            }
            i++;
        }
    }
    char *f1_name = argv[1];
    char *f2_name = argv[2];
    FILE *pfile1 = fopen(f1_name, "r");
    if (pfile1 == NULL)
    {
        fprintf(stderr, "Could not open file: %s\n", f1_name);
        exit(1);
    }
    FILE *pfile2 = fopen(f2_name, "r");
    if (pfile2 == NULL)
    {
        fprintf(stderr, "Could not open file: %s\n", f2_name);
        fclose(pfile1);
        exit(1);
    }
    int result = compare_files(pfile1, pfile2, ignore_case, verbose);
    if (fclose(pfile1) != 0) {
        fprintf(stderr, "Error closing file: %s\n", f1_name);
        fclose(pfile2);
        exit(1);
    }
    if (fclose(pfile2) != 0) {
        fprintf(stderr, "Error closing file: %s\n", f2_name);
        exit(1);
    }
    return result;
}
