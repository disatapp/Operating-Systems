// Pavin Disatapundhu
// disatapp@onid.oregonstate.edu
// CS344-400
// homework#3 Question2
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>     
#include <sys/types.h>
#include <signal.h>

void signal1_handler(int s){
    printf(s," has been caught\n");
}

void signal2_handler(int s){
    printf(s," has been caught\n");
}

void signal3_handler(int s){
    printf(s," has been caught, terminating the program\n");
    exit(EXIT_FAILURE);
}

int main(int argv, char *argc[]){
    pid_t mypid = getpid();
    //signusr1
    if (signal(SIGUSR1, signal1_handler) == SIG_ERR) {
        printf("An error has occour while handling SIGUSR1\n");
        exit(EXIT_FAILURE);
    }
    //signusr2
    if (signal(SIGUSR2, signal2_handler) == SIG_ERR) {
        printf("An error has occour while handling SIGUSR2\n");
        exit(EXIT_FAILURE);
    }
    //sigint
    if (signal(SIGINT, signal3_handler) == SIG_ERR) {
        printf("An error has occour while handling SIGINT\n");
        exit(EXIT_FAILURE);
    }
    
    kill(mypid, SIGUSR1);
    kill(mypid, SIGUSR2);
    kill(mypid, SIGINT);
    
    return 0; //EXIT_SUCCESS = 0
}
    
//http://en.cppreference.com/w/c/program/SIG_ERR