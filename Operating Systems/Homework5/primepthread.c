//name: Pavin Disatapundhu
//e-mail: disatapp@onid.oregonstate.edu
//class: CS344-400
//assignment: Homework #5, primemproc.c

#define _XOPEN_SOURCE
#define _BSD_SOURCE

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

//set macros
#define BITCOUNT 32
#define ARRBIT(b) (b/BITCOUNT)
#define MODBIT(b) (b%BITCOUNT)
//compare funciton
#define TESTBIT(a,b)    (a[ARRBIT(b)] & (1 << MODBIT(b)))
#define SETBIT(a,b)     (a[ARRBIT(b)] |= (1 << MODBIT(b)))
#define CLEARBIT(a,b)   (a[ARRBIT(b)] &= ~(1 << MODBIT(b)))
//http://www.mathcs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html

int validateinput(int argument);
void initialize(void);
size_t validatesize(size_t argument);
void printallprimes(void);
void mpthread(void);
void *eratosthenes(void *field);
int quitf = 1;
int proc = 1;
size_t *sieve;
size_t *threadstack;
size_t size = UINT_MAX;


int main(int argc, char **argv){
    
    char c;
    
    size_t object_int;
    size_t object_size;
    clock_t starttime, stoptime;
    double runtime;
    
    while ((c = getopt (argc, argv, "q:m:c:")) != -1){
        switch (c){
            case 'q':
                quitf = 0;
                break;
            case 'm':
                size = atoi(optarg);
                break;
            case 'c':
                proc = atoi(optarg);
                break;
            default:
                printf("usage: ./primemproc [-q] [-m] max # to calculate [-c] max # of process");
                //http://courses.cms.caltech.edu/cs11/material/general/usage.html
                break;
        }
    }
    size = validatesize(size);
    proc = validateinput(proc);
    starttime = clock();
    object_int = (int)((size / BITCOUNT) + 1);
    object_size = object_int * sizeof(size_t);
    threadstack = malloc(object_size);
    //from lecture http://web.engr.oregonstate.edu/~chaneyr/lectures/18-shared_memory.mp4
    
    //initialize threadstack
    initialize();
    
    //thread
    mpthread();
    
    stoptime = clock();
    runtime = (double)(stoptime - starttime)/CLOCKS_PER_SEC;
    //print
    printf("Program completed!\n");
    printf("Runtime %f seconds\n", runtime);
    
    //unmap and unlink shared memory object
    free(threadstack);
    return 0;
}

// this funciton will make sure the inputs are within the bounderies
int validateinput(int argument){
    if (argument < 1){
        fprintf(stderr,"Min process set to 1.\n");
        proc = 1;
    }else if (argument > 10){
        fprintf(stderr,"Max process set to 10.\n");
        proc = 10;
    }
    return proc;
}

// this funciton will make sure the inputs are within the bounderies
size_t validatesize(size_t argument){
    if (argument < 5){
        fprintf(stderr,"Min number set to 5.\n");
        argument = 5;
    }
    return argument;
}

//intialize
void initialize(void){
    size_t i;
    //bit-array set to one
    for (i = 0; i < size; i++ ){
        SETBIT(threadstack, i);
    }
    CLEARBIT(threadstack, 0);
    CLEARBIT(threadstack, 1);
    //http://www.mathcs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html
    
}

//mpthread is used to fork into mulitple-threading.
void mpthread(void){
    size_t thread;
    pthread_t *thread_handle;
    
    //In this function we takes the current process count and divide it by the
    sieve = (size_t*)malloc(proc * sizeof(size_t));
    for (thread = 0; thread < proc; thread++){
        sieve[thread] = ceil((double)(thread + 1)/proc * sqrt(size));
    }
    thread_handle = (pthread_t*)malloc(sizeof(pthread_t) * proc);
    //create thread
    for (thread = 0; thread < proc; thread++){
        if(pthread_create(&thread_handle[thread], NULL, eratosthenes, (void *)(long int)thread) != 0){
            perror("thread");
            exit(EXIT_FAILURE);
        }
    }
    printf("Main thread\n");
    
    for (thread = 0; thread < proc; thread++){
        pthread_join(thread_handle[thread], NULL);
    }
    //http://stackoverflow.com/questions/8618637/what-does-it-mean-to-convert-int-to-void-or-vice-versa
    
    if (quitf != 0){
        printallprimes();
    }

    
    free(thread_handle);
}
//print out all the paired primes
void printallprimes(void){
    size_t pair;
    int count = 1;
    for (size_t i = 2; i < (size - 1); i++){
        if (TESTBIT(threadstack, i)){
            pair = i + 2;
            if(TESTBIT(threadstack, pair)){
                printf("Pair %d: (%d,%d)\n", count,(int)i, (int)pair);
                count++;
            }
        }
    }
}

//seive function this takes only one argument because the create function allows only one parameter.
void *eratosthenes(void *threadnum){
    size_t i;
    size_t min;
    size_t max;
    size_t offset = 0;
    size_t ops = (long int)threadnum;
    long long z;
    
    
    for (i = 0; i < (ops+1); i++){
        max = offset + sieve[i];
        min = offset + 1;
        offset += sieve[i];
        if (sqrt(size) < max){
            max = sqrt(size);
        }
    }
    
    //http://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
    for(i = min; i < (max+1); i++){
        if(TESTBIT(threadstack, i)){
            //z = i^2
            // all values that are mulitles of z should be marked
            for (z = i * i; z < size; z += i){
                if(TESTBIT(threadstack, z)){
                    CLEARBIT(threadstack, z);
                }
            }
        }
    }
    return 0;
}
