//name: Pavin Disatapundhu
//e-mail: disatapp@onid.oregonstate.edu
//class: CS344-400
//assignment: Homework #4, uniqify.c

#define _XOPEN_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#define MAX_WORD_LENGTH 100

void limit(int proc);
void makepipes(int proc, int **arr);
void freeArray(int **inpipes, int **outpipes, int proc);
void parser(int proc, int** inpipes);
void sort(int proc, int **inpipes, int **outpipes);
void combiner(int proc, int** outpipes, FILE* fparent);

static void handlech(int sig){
    while(waitpid(-1, NULL, WNOHANG) > 0){
        continue;
    }
    //http://stackoverflow.com/questions/7171722/how-can-i-handle-sigchld-in-c
}

static void handleint(int sig){
    exit(-1);
}

int main(int argc, char *argv[]){
    FILE *fparent= stdout;
    //http://stackoverflow.com/questions/3551125/c-file-pointer-to-stdout
    int i;
    char c;
    int proc = 0;
    int **inpipes;
    int **outpipes;
    struct sigaction sig_ch, sig_int;
    
    //ready signals
    sigemptyset(&sig_ch.sa_mask);
    sig_ch.sa_flags = 0;
    sig_int.sa_flags = 0;
    sig_ch.sa_handler = handlech;
    sig_int.sa_handler = handleint;
    sigaction(SIGCHLD, &sig_ch, NULL);
    sigaction(SIGINT, &sig_int, NULL);
    
    //http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
    
    while ((c = getopt (argc, argv, "n:")) != -1){
        switch (c){
            case 'n':
                proc = atoi(argv[2]);
                break;
            default:
                break;
        }
    }
    //Making pipes
    limit(proc);
    outpipes = (int**)malloc(proc * sizeof(int*));
    inpipes = (int**)malloc(proc * sizeof(int*));
    
    for (i = 0; i < proc; i++){
        inpipes[i] = malloc(2 * sizeof(int));
        outpipes[i] = malloc(2 * sizeof(int));
    }
    makepipes(proc, outpipes);
    makepipes(proc, inpipes);
    
    printf("parser");
    parser(proc, inpipes);
    printf("sort");
    //sort it
    sort(proc, inpipes, outpipes);
//    printf("com");
//    //fork process, if it's a child combine it.
    combiner(proc, outpipes, fparent);
//

    freeArray(inpipes, outpipes, proc);
    
    
    while(wait(NULL) > 0);
    return 0;
}

void limit(int proc){
    //at least 1 process
    if (proc < 1){
        fprintf(stderr,"Setting number of process to 1.\n");
        proc = 1;
    }
    //50 process max
    if (proc > 50){
        fprintf(stderr,"The limiting number of processes to 50.\n");
        proc = 50;
    }
}

void makepipes(int proc, int **arr){
    int i;
    for(i = 0; i < proc; i++){
        if(pipe(arr[i]) == -1){
            perror("Fail to create pipe.");
            exit(1);
        }
    }
}

//free memory of multidimensinal array
void freeArray(int **inpipes, int **outpipes, int proc){
    int i;
    for (i = 0; i < proc; i++){
        free(outpipes[i]);
        free(inpipes[i]);
    }
    free(outpipes);
    free(inpipes);
    //http://stackoverflow.com/questions/1733881/c-correctly-freeing-memory-of-a-multi-dimensional-array
}

void parser(int proc, int** inpipes){
    
    FILE *fd[proc];
    char str[MAX_WORD_LENGTH];
    int i, j, k;
    
    //open the fp stream to write
    for (i = 0; i < proc; i++){
        fd[i] = fdopen(inpipes[i][1], "w");
    }
    
    //stdin parsing the line to words, and convert to lower
    k = 0;
    while (fscanf(stdin, " %[a-zA-Z]s", str) != EOF){
        for (j = 0; j < strlen(str); j++){
            str[j] = tolower(str[j]);
            fputs(str, fd[k]);
            fputs("\n",fd[k]);
            k = (k + 1) % proc;
        }
        fgetc(stdin);
    }
    
    //close all fd stream
    for (i = 0; i < proc; i++){
        fclose(fd[i]);
    }
    //http://stackoverflow.com/questions/2661766/c-convert-a-mixed-case-string-to-all-lower-case
    
}

//A group of sort sub-processes process does the sorting
void sort(int proc, int **inpipes, int **outpipes){
    
    int temp1, temp2;

    temp1 = dup(STDIN_FILENO);
    temp2 = dup(STDOUT_FILENO);
    
    for (int i = 0; i < proc; i++){
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        
        //fork child processes
        switch(fork()){
            case 0:
                if (dup2(inpipes[i][0], STDIN_FILENO) == -1){
                    perror("Duplicate");
                    exit(EXIT_FAILURE);
                }
                //dup input pipe and send to out pipe
                if (dup2(outpipes[i][1], STDOUT_FILENO) == -1){
                    perror("Duplicate");
                    exit(EXIT_FAILURE);
                }
                for (int j = 0; j <= i; j++) {
                    close(inpipes[j][0]);
                    close(outpipes[j][0]);
                    close(inpipes[j][1]);
                    close(outpipes[j][1]);
                }
                //execute the sorting process
                if(execlp("sort", "sort", (char*) NULL)== -1){
                    perror("Execlp fail");
                    _exit(EXIT_FAILURE);
                }
                //http://en.wikipedia.org/wiki/Dup_(system_call)
                
                break;
            case -1:
                perror("Fail to create child");
                exit(EXIT_FAILURE);
            default:
                break;
        }
        close(inpipes[i][0]);
        close(outpipes[i][1]);
    }
    dup2(temp1, STDIN_FILENO);
    dup2(temp2, STDOUT_FILENO);
}


//A combiner stage that suppresses duplicate words, counts occurrences, and writes the output to stdout (this should be its own process).
void combiner(int proc, int** outpipes, FILE* fparent){
    FILE* fd[proc];
    int i, j, k, flag;
    int first = 1;
    char compare[MAX_WORD_LENGTH];
    char** word;
    fparent = stdout;
    
    switch(fork()){
        case 0:
            flag = 1;
            j = 1;
            k = 0;
            word = (char**)malloc(proc * sizeof(char*));
            for (i = 0; i < proc; i++){
                word[i] = (char*)malloc(MAX_WORD_LENGTH * sizeof(char));
                fd[i] = fdopen(outpipes[i][0], "r");
                if(fgets(word[i], MAX_WORD_LENGTH, fd[i]) == NULL){
                    word[i] = NULL;
                }
            }
            
            while (flag){
                for (i = 0; i < proc; i++){
                    if (word[i] != NULL){
                        if (strcmp(word[i], word[k]) < 0){
                            k = i;
                        }
                    }
                }
                
                if (first == 1){
                    strcpy(compare, word[k]);
                    j = 0;
                    first = 0;
                }
                if (strcmp(word[k], compare) == 0){
                    j++;
                }else{
                    fprintf(fparent, "%s = %d", compare, j);
                    strcpy(compare, word[k]);
                    j = 1;
                }
                
                if(fgets(word[k], MAX_WORD_LENGTH, fd[k]) == NULL){
                    word[k] = NULL;
                }
                
                for (i = 0; i < proc; i++){
                    if (word[i] != NULL){
                        k = i;
                        break;
                    } else {
                        flag = 0;
                    }
                }
            }

            fprintf(fparent, "%s %d",compare, j);
            
            //close
            for (i = 0; i < proc; i++){
                fclose(fd[i]);
                free(fd[i]);
            }
            fclose(fparent);
            free(fd);
        case -1:
            perror("Fail to create child");
            exit(EXIT_FAILURE);
        default:
            printf("I am the parent, and my process ID is %d\n", getpid());
            break;
    }
}