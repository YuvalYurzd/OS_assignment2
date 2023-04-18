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

void run_command_with_pipes(char *command1, char *command2, char *command3) {
    int pipe1[2]; // Pipe for command1 to command2
    int pipe2[2]; // Pipe for command2 to command3

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Failed to create pipes");
        return;
    }

    pid_t pid1, pid2;

    pid1 = fork();
    if (pid1 == 0) {
        dup2(pipe1[1], STDOUT_FILENO);

        // Close unnecessary file descriptors
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp(command1, command1, NULL);
        perror("Failed to execute command1");
        exit(1);
    } else if (pid1 < 0) {
        perror("Failed to fork for command1");
        return;
    }
    pid2 = fork();
    if (pid2 == 0) {
        dup2(pipe1[0], STDIN_FILENO);
        dup2(pipe2[1], STDOUT_FILENO);

        // Close unnecessary file descriptors
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp(command2, command2, NULL);
        perror("Failed to execute command2");
        exit(1);
    } else if (pid2 < 0) {
        perror("Failed to fork for command2");
        return;
    }

    pid_t pid3 = fork();
    if (pid3 == 0) {
        dup2(pipe2[0], STDIN_FILENO);

        // Close unnecessary file descriptors
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp(command3, command3, NULL);
        perror("Failed to execute command3");
        exit(1);
    } else if (pid3 < 0) {
        perror("Failed to fork for command3");
        return;
    }

    // Close all file descriptors in the parent process
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    // Wait for all child processes to finish
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);
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
            if (command[j] == ' ')
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
                run_command_with_pipes(token, token2, token3);
                num_of_pipes = 0;
                free(token);
                free(token3);
                continue;
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
