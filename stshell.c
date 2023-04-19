#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define INPUTSIZE 1024
#define MAXARGS 10

void handle_signal(int sig)
{
    printf("\n"); // print a newline after the ^C signal
}

void tokenizeString(const char *input, char *args[], int maxNumArgs) {
    char *token = strtok((char *)input, " ");
    int i = 0;

    while (token != NULL && i < maxNumArgs) {
        args[i++] = strdup(token);
        token = strtok(NULL, " ");
    }

    // Fill remaining array elements with NULL
    while (i < maxNumArgs) {
        args[i++] = NULL;
    }
}



int main()
{
    char command[INPUTSIZE];
    char *args[MAXARGS];

    // in case of 2 pipe command those tokens will hold the values of the commands
    char *token; 
    char *token2;
    char *token3;

    int i;
    int redirect_mode = 0; // 0: no redirection, 1: overwrite (>), 2: append (>>)
    char *redirect_file;
    int num_of_pipes = 0;

    // set up signal handler for ^C
    signal(SIGINT, handle_signal);

    // shell loop
    while (1)
    {
        printf(">: ");
        fgets(command, sizeof(command), stdin);
        int k = 0;
        for (int j = strlen(command); j >= 0; j--)
        {
            if (command[j] == ' ' && command[j - 1] == '|')
                break;
            k++;
        }
        int x = strlen(command) - k;
        int index = 0;
        token3 = (char *)malloc(sizeof(char) * k + sizeof(char));
        for (int j = x + 1; j < strlen(command) - 1; j++)
        {
            token3[index] = command[j];
            index++;
        }
        token3[index] = '\0';
        for (int i = 0; i < strlen(command); i++)
        {
            if (command[i] == '|')
                num_of_pipes++;
        }
        int t = 0;
        for(int j = 0; j < strlen(command); j++)
        {
            if(command[j] == '|')
                break;
            if(command[j] == ' ' && command[j+1] == '|')
                break;
            t++;
        }
        int index2 = 0;
        token = (char *)malloc(sizeof(char) * t + sizeof(char));
        for(int j = 0; j < t; j++)
        {
            token[index2] = command[j];
            index2++;
        }
        token[index2] = '\0';

        command[strcspn(command, "\n")] = 0; // remove \n

        // check if user wants to exit the shell
        if (strcmp(command, "exit") == 0)
        {
            exit(0);
        }

        i = 0;
        token2 = strtok(command, " ");

        // check for pipes and redirections
        while (token2 != NULL)
        {
            if (strcmp(token2, ">") == 0)
            {
                redirect_mode = 1;
                token2 = strtok(NULL, " ");
                redirect_file = token2;
                break;
            }
            else if (strcmp(token2, ">>") == 0)
            {
                redirect_mode = 2;
                token2 = strtok(NULL, " ");
                redirect_file = token2;
                break;
            }
            else if (strcmp(token2, "|") == 0)
            {
                token2 = strtok(NULL, " ");
                break;
            }
            else
            {
                args[i] = token2;
                token2 = strtok(NULL, " ");
                i++;
            }
        }
        args[i] = NULL;
        if (args[0] == NULL)
        { // empty command was entered
            continue;
        }
        pid_t pid = fork();
        if (pid < 0)
        {
            printf("Fork failed, closing shell..");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            signal(SIGINT, SIG_DFL);
            if (num_of_pipes == 1)
            {
                int pipefd[2];
                pid_t pid2;
                pipe(pipefd);
                pid2 = fork();
                if (pid2 == 0)
                {
                    close(pipefd[0]);            
                    dup2(pipefd[1], STDOUT_FILENO); 
                    execvp(args[0], args);
                    perror("execvp failed");
                    exit(EXIT_FAILURE);
                }
                else if (pid2 < 0)
                {
                    perror("fork failed");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    close(pipefd[1]);              
                    dup2(pipefd[0], STDIN_FILENO); 
                    wait(NULL);

                    
                    char *args2[MAXARGS];
                    args2[0] = token2; 
                    args2[1] = NULL;  
                    execvp(args2[0], args2);
                    perror("execvp failed");
                    exit(EXIT_FAILURE);
                }
            }
            if (num_of_pipes == 2)
            {
                int pipefd[4];

                if(pipe(pipefd) == -1 || pipe(pipefd + 2) == -1)
                {
                    printf("error creating pipe");
                    exit(EXIT_FAILURE);
                }

                pid_t pid2 = fork();

                if(pid < 0)
                {
                        printf("fork failed");
                        exit(EXIT_FAILURE);
                }

                // first command
                else if(pid2 == 0)
                {
                    dup2(*(pipefd + 1), STDOUT_FILENO);
                    close(*pipefd);
                    close(*(pipefd + 1));
                    close(*(pipefd + 2));
                    close(*(pipefd + 3));


                    char *args[MAXARGS];
                    tokenizeString(token, args, MAXARGS);
                    if(execvp(args[0], args) == - 1)
                    {
                        perror("execvp failed");
                        exit(EXIT_FAILURE);
                    }

                    exit(EXIT_SUCCESS);
                }
                pid2 = fork();
                if(pid2 < 0)
                {
                    printf("fork failed");
                    exit(EXIT_FAILURE);
                }

                //second command
                else if (pid2 == 0)
                {
                    dup2(*pipefd, STDIN_FILENO);
                    dup2(*(pipefd + 3), STDOUT_FILENO);
                    close(*pipefd);
                    close(*(pipefd + 1));
                    close(*(pipefd + 2));
                    close(*(pipefd + 3));

                    char *args2[MAXARGS];
                    tokenizeString(token2, args2, MAXARGS);
                    if(execvp(args2[0],args2) == - 1)
                    {
                        perror("execvp failed");
                        exit(EXIT_FAILURE);
                    }

                    exit(EXIT_SUCCESS);
                }

                //third command
                else {
                    dup2(*(pipefd + 2), STDIN_FILENO);
                    close(*pipefd);
                    close(*(pipefd + 1));
                    close(*(pipefd + 2));
                    close(*(pipefd + 3));

                    char *args3[MAXARGS];
                    tokenizeString(token3, args3, MAXARGS);
                    if(execvp(args3[0],args3) == - 1)
                    {
                        perror("execvp failed");
                        exit(EXIT_FAILURE);
                    }
                }
            }

            if (redirect_mode == 1)
            {
                int fd = open(redirect_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1)
                {
                    perror("open failed");
                    exit(EXIT_FAILURE);
                    perror("execvp failed");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
            }
            else if (redirect_mode == 2)
            {
                int fd = open(redirect_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd == -1)
                {
                    perror("open failed");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
            }
            execvp(args[0], args);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
        else
        {
            wait(NULL);
        }

        redirect_mode = 0; // reset redirection mode and file
        redirect_file = NULL;
        num_of_pipes = 0;
    }
    return 0;
}
