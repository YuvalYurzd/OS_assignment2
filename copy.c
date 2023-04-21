#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    // Check if correct number of arguments provided
    if (argc < 3 || argc > 4)
    {
        printf("Usage: ./copy <source_file> <destination_file> [-v] [-f]\n");
        return 1;
    }

    // Get source file and destination file paths
    char *source_file = argv[1];
    char *destination_file = argv[2];

    // Check if -v flag provided
    int verbose = 0;
    if (argc == 4 && strcmp(argv[3], "-v") == 0)
    {
        verbose = 1;
    }

    // Check if -f flag provided
    int force = 0;
    if (argc == 4 && strcmp(argv[3], "-f") == 0)
    {
        force = 1;
    }

    // Check if destination file already exists
    FILE *dest_file = fopen(destination_file, "r");
    if (dest_file != NULL)
    {
        if (!force)
        {
            printf("target file exists\n");
            fclose(dest_file);
            return 1;
        }
        fclose(dest_file);
    }

    // Open source file for reading
    FILE *src_file = fopen(source_file, "r");
    if (src_file == NULL)
    {
        if (verbose)
        {
            printf("general failure\n");
        }
        return 1;
    }

    // Open destination file for writing
    dest_file = fopen(destination_file, "w");
    if (dest_file == NULL)
    {
        if (verbose)
        {
            printf("general failure\n");
        }
        fclose(src_file);
        return 1;
    }

    // Copy content from source file to destination file
    int ch;
    while ((ch = fgetc(src_file)) != EOF)
    {
        fputc(ch, dest_file);
    }

    fclose(src_file);
    fclose(dest_file);

    // Print success message if -v flag provided
    if (verbose)
    {
        printf("success\n");
    }

    return 0;
}
