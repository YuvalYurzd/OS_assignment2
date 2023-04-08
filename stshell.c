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
    printf("\n");  // print a newline after the ^C signal
}

int main()
{
    char command[INPUTSIZE];
    char *args[MAXARGS];
    char *token;
    int i;
    int redirect_mode = 0; // 0: no redirection, 1: overwrite (>), 2: append (>>)
    char *redirect_file;

    // set up signal handler for ^C
    signal(SIGINT, handle_signal);

    while (1){
        printf(">: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0; //remove \n
        if(strcmp(command, "exit") == 0){
            exit(0);
        }
        i = 0;
        token = strtok(command, " ");
        while (token != NULL){
            if(strcmp(token, ">") == 0){
                redirect_mode = 1;
                token = strtok(NULL, " ");
                redirect_file = token;
                break;
            }
            else if(strcmp(token, ">>") == 0){
                redirect_mode = 2;
                token = strtok(NULL, " ");
                redirect_file = token;
                break;
            }
            else{
                args[i] = token;
                token = strtok(NULL, " ");
                i++;
            }
        }

        args[i] = NULL;
        if(args[0] == NULL){ // empty command was entered
            continue;
        }

        pid_t pid = fork();
        if(pid < 0){
            printf("Fork failed, closing shell..");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0){
            if(redirect_mode == 1){
                int fd = open(redirect_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if(fd == -1){
                    perror("open failed");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
            }
            else if(redirect_mode == 2){
                int fd = open(redirect_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                if(fd == -1){
                    perror("open failed");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
            }
            execvp(args[0], args);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else{
            wait(NULL);
        }

        redirect_mode = 0; // reset redirection mode and file
        redirect_file = NULL;
    }
}
