#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "usage: ./copy <file1> <file2> [-v] [-f]\n");
        return 1;
    }

    int verbose = 0;
    int force = 0;

    if (argc > 3)
    {
        for (int i = 3; i < argc; i++)
        {
            if (strcmp(argv[i], "-v") == 0)
            { // checking if -v was entered by user
                verbose = 1;
            }
            else if (strcmp(argv[i], "-f") == 0)
            { // checking if -f was entered by user
                force = 1;
            }
            else
            {
                fprintf(stderr, "Invalid option: %s\n", argv[i]);
                return 1;
            }
        }
    }

    char *source_file_name = argv[1];
    char *dest_file_name = argv[2];

    FILE *psource_file = fopen(source_file_name, "r");
    if (psource_file == NULL)
    {
        fprintf(stderr, "Could not open source file: %s\n", source_file_name);
        return 1;
    }

    // creating destination file or checking if -f was entered by user and overwriting the file if needed
    FILE *pdest_file = fopen(dest_file_name, "r");
    if (pdest_file != NULL)
    {
        if (fclose(pdest_file) != 0)
        {
            fprintf(stderr, "Error\n");
            fclose(psource_file);
            return 1;

            if (force)
            {
                pdest_file = fopen(dest_file_name, "w");
            }
            else
            {
                fprintf(stderr, "Target file exists: %s\n", dest_file_name);
                fclose(psource_file);
                return 1;
            }
        }
        else
        {
            pdest_file = fopen(dest_file_name, "w");
        }

        if (pdest_file == NULL)
        {
            fprintf(stderr, "Could not open destination file: %s\n", dest_file_name);
            fclose(psource_file);
            return 1;
        }

        int ch = 0;

        // writing into destination file
        while ((ch = fgetc(psource_file)) != EOF)
        {
            if (fputc(ch, pdest_file) == EOF)
            {
                fprintf(stderr, "Error writing to destination file: %s\n", dest_file_name);
                fclose(psource_file);
                fclose(pdest_file);
                return 1;
            }
        }

        if (fclose(psource_file) != 0)
        {
            fprintf(stderr, "Error closing source file: %s\n", source_file_name);
            fclose(pdest_file);
            return 1;
        }

        if (fclose(pdest_file) != 0)
        {
            fprintf(stderr, "Error closing destination file: %s\n", dest_file_name);
            return 1;
        }

        if (verbose)
        {
            printf("Success\n");
        }

        return 0;
    }
    return 0;
}