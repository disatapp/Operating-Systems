//name: Pavin Disatapundhu
//email: disatapp@onid.oregonstate.edu
//class: CS344-400
//assignment: Homework #4, pipeline

#define _XOPEN_SOURCE //optarg

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <getopt.h>
#include <errno.h>


void pipeline(char* file);

int main(int argc, char *argv[]){
    char c;
    
    while ((c = getopt (argc, argv, "f:")) != -1){
        switch (c){
            case 'f':
                pipeline(optarg);
                break;
            default:
                break;
        }
    }
    //http://linux.die.net/man/3/optarg
    return 0;
}

void pipeline(char* file){
    int number = 4;
    int fd[number][2];
    int i, k, j;

    for (i = 0; i < 5; i++)
    {
        if (i < number)
            if (pipe(fd[i]) == -1){
                perror("Call a plumber");
                exit(EXIT_FAILURE);
            }
        switch(fork()){
            case 0:
                if (i == 0){
                    if (fd[i][1] != STDOUT_FILENO){
                        dup2(fd[i][1], STDOUT_FILENO);
                    }
                } else if (i == 4){
                    if (fd[i-1][0] != STDIN_FILENO){
                        dup2(fd[i-1][0], STDIN_FILENO);
                    }
                } else {
                    if (fd[i-1][0] != STDIN_FILENO){
                        dup2(fd[i-1][0], STDIN_FILENO);
                    }
                    if (fd[i][1] != STDOUT_FILENO){
                        dup2(fd[i][1], STDOUT_FILENO);
                    }
                }
                for (j = 0; j < i; j++){
                    if (close(fd[j][0]) == -1){
                        perror("Closing error");
                        exit(EXIT_FAILURE);
                    }
                    if (close(fd[j][1]) == -1){
                        perror("Closing error");
                        exit(EXIT_FAILURE);
                    }
                }			
                
                switch(j){
                    case 0:
                        execlp("rev", "rev", NULL);
                    case 1:
                        execlp("sort", "sort", NULL);
                    case 2:
                        execlp("uniq", "uniq", "-c", NULL);
                    case 3:
                        execlp("tee", "tee", file, NULL);
                    case 4:
                        execlp("wc", "wc", NULL);
                }
                break;
            case -1:
                perror("Forking child");
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }
    //http://beej.us/guide/bgipc/output/html/multipage/pipes.html
    
    //close
    for (i = 0; i < number; i++){
        if (close(fd[i][0]) == -1){
            perror("Closing error");
            exit(1);
        }
        if (close(fd[i][1]) == -1){
            perror("Closing error");
            exit(1);
        }		
    }
    
    //kill them
    while (k < 5){
        if (wait(NULL) == -1){
            exit(1);
        }
        k++;
    }
}