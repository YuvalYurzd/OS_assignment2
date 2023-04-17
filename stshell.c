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

int main()
{
    char command[INPUTSIZE];
    char *args[MAXARGS];
    char *token;
    int i;
    int redirect_mode = 0; // 0: no redirection, 1: overwrite (>), 2: append (>>)
    char *redirect_file;
    int num_of_pipes = 0;
    int pipearr[2];

    // set up signal handler for ^C
    signal(SIGINT, handle_signal);

    while (1)
    {
        printf(">: ");
        fgets(command, sizeof(command), stdin);
        for (int i = 0; i < strlen(command); i++)
        {
            if (command[i] == '|')
                num_of_pipes++;
        }
        command[strcspn(command, "\n")] = 0; // remove \n
        if (strcmp(command, "exit") == 0)
        {
            exit(0);
        }
        i = 0;
        token = strtok(command, " ");
        while (token != NULL)
        {
            if (strcmp(token, ">") == 0)
            {
                redirect_mode = 1;
                token = strtok(NULL, " ");
                redirect_file = token;
                break;
            }
            else if (strcmp(token, ">>") == 0)
            {
                redirect_mode = 2;
                token = strtok(NULL, " ");
                redirect_file = token;
                break;
            }
            else if (strcmp(token, "|") == 0)
            {
                token = strtok(NULL, " ");
                break;
            }
            else
            {
                args[i] = token;
                token = strtok(NULL, " ");
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
            char *args2[MAXARGS];
            if (num_of_pipes == 1)
            {
                printf("1 pipe\n");
                int pipefd[2];
                pid_t pid2;
                pipe(pipefd);
                pid2 = fork();
                if (pid2 == 0)
                {
                    close(pipefd[0]);               // close reading end of the pipe
                    dup2(pipefd[1], STDOUT_FILENO); // redirect stdout to the writing end of the pipe
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
                    close(pipefd[1]);              // close writing end of the pipe
                    dup2(pipefd[0], STDIN_FILENO); // redirect stdin to the reading end of the pipe
                    wait(NULL);

                    // Update: Initialize args2 with NULL-terminated array
                    char *args2[MAXARGS];
                    args2[0] = token; // use the first argument after the pipe symbol as the new command
                    args2[1] = NULL;  // terminate the arguments array
                    execvp(args2[0], args2);
                    perror("execvp failed");
                    exit(EXIT_FAILURE);
                }
            }
            else if (num_of_pipes == 2)
            {
                printf("2 pipes\n");
                int pipefd[2];
                int pipefd2[2];
                pid_t pid2, pid3;

                pipe(pipefd);
                pipe(pipefd2);

                pid2 = fork();
                if (pid2 == 0)
                {
                    close(pipefd[0]);               // close reading end of the first pipe
                    dup2(pipefd[1], STDOUT_FILENO); // redirect stdout to the writing end of the first pipe
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
                    close(pipefd[1]);              // close writing end of the first pipe
                    dup2(pipefd[0], STDIN_FILENO); // redirect stdin to the reading end of the first pipe

                    pid3 = fork();
                    if (pid3 == 0)
                    {
                        close(pipefd2[0]);               // close reading end of the second pipe
                        dup2(pipefd2[1], STDOUT_FILENO); // redirect stdout to the writing end of the second pipe
                        execvp(args2[0], args2);
                        perror("execvp failed");
                        exit(EXIT_FAILURE);
                    }
                    else if (pid3 < 0)
                    {
                        perror("fork failed");
                        exit(EXIT_FAILURE);
                    }
                    else
                    {
                        close(pipefd2[1]);              // close writing end of the second pipe
                        dup2(pipefd2[0], STDIN_FILENO); // redirect stdin to the reading end of the second pipe
                        wait(NULL);
                        wait(NULL);
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
