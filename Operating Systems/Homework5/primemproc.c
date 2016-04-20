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

//set macros
#define BITCOUNT 32
#define Mylink "/disatapplink"
#define ARRBIT(b) (b/BITCOUNT)
#define MODBIT(b) (b%BITCOUNT)
//compare funciton
#define TESTBIT(a,b)    (a[ARRBIT(b)] & (1 << MODBIT(b)))
#define SETBIT(a,b)     (a[ARRBIT(b)] |= (1 << MODBIT(b)))
#define CLEARBIT(a,b)   (a[ARRBIT(b)] &= ~(1 << MODBIT(b)))
//http://www.mathcs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html

int validateinput(int proc);
void initialize(size_t *bitmap, size_t size);
size_t validatesize(size_t size);
void mpfork(size_t *bitmap, size_t size, size_t object_size, void *mount, int proc);
void *mount_shmem(char *path, size_t object_size);
void eratosthenes(size_t *bitmap, size_t size, int ops, int proc, size_t *sieve);
void printallprimes(size_t *bitmap, size_t size);

int main(int argc, char **argv){
    
    char c;
    
    int proc = 1;
    int quitf = 1;
    void *mount;
    double runtime;
    size_t object_int;
    size_t object_size;
    size_t *bitmap;
    size_t size = UINT_MAX;
    clock_t starttime, stoptime;
    
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
    
    mount = mount_shmem(Mylink, object_size);
    bitmap = (size_t*)mount;
    //from lecture http://web.engr.oregonstate.edu/~chaneyr/lectures/18-shared_memory.mp4
    
    //initialize bitmap
    initialize(bitmap, size);
    
    //fork
    mpfork(bitmap, size, object_size, mount, proc);
    
    //print twin primes, dont print if quit is set
    if (quitf != 0){
        printallprimes(bitmap, size);
    }
    
    stoptime = clock();
    runtime = (double)(stoptime - starttime)/CLOCKS_PER_SEC;
    //print
    printf("Program completed!\n");
    printf("Runtime %f seconds\n", runtime);
    
    //unmap and unlink shared memory object
    munmap(mount, object_size);
    shm_unlink(Mylink);
    
    return 0;
}

// this funciton will make sure the inputs are within the bounderies
int validateinput(int proc){
    if (proc < 1){
        fprintf(stderr,"Min process set to 1.\n");
        proc = 1;
    }else if (proc > 10){
        fprintf(stderr,"Max process set to 10.\n");
        proc = 10;
    }
    return proc;
}
// this funciton will make sure the inputs are within the bounderies
size_t validatesize(size_t size){
    if (size < 5){
        fprintf(stderr,"Min number set to 5.\n");
        size = 5;
    }
    return size;
}


//intialize
void initialize(size_t *bitmap, size_t size){
    size_t i;
    //bit-array set to one
    for (i = 0; i < size; i++ ){
        SETBIT(bitmap, i);
    }
    CLEARBIT(bitmap, 0);
    CLEARBIT(bitmap, 1);
    //http://www.mathcs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html
}

//mpfork is used to fork into mulitple process.
void mpfork(size_t *bitmap, size_t size, size_t object_size, void *mount, int proc){
    
    size_t currentproc;
    size_t *sieve;
    size_t pid;
    
    //In this function we takes the current process count and divide it into equal segments
    sieve = (size_t*)malloc(proc * sizeof(size_t));
    for (currentproc = 0; currentproc < proc; currentproc++){
        sieve[currentproc] = ceil((double)(currentproc + 1)/proc * sqrt(size));
    }
    for (currentproc = 0; currentproc < proc; currentproc++){
        switch(pid = fork()){
            case -1:
                perror("Child");
                exit(EXIT_FAILURE);
            case 0:
                eratosthenes(bitmap, size, currentproc, proc, sieve);
                munmap(mount, object_size);
                exit(1);
                break;
            default:
                break;
        }
    }
    //kill all the children
    for (currentproc = 0; currentproc < proc; currentproc++){
        wait(NULL);
    }
    //http://stackoverflow.com/questions/21863840/making-children-processes-wait-for-another-for-loop
    
}

void printallprimes(size_t *bitmap, size_t size){
    size_t pair;
    int count = 1;
    for (size_t i = 2; i < (size - 1); i++){
        if (TESTBIT(bitmap, i)){
            pair = i + 2;
            if(TESTBIT(bitmap, pair)){
                printf("Pair %d: (%d,%d)\n", count,(int)i, (int)pair);
                count++;
            }
        }
    }
}

//seive function
void eratosthenes(size_t *bitmap, size_t size, int ops, int proc, size_t *sieve){
    size_t i;
    size_t min;
    size_t max;
    size_t offset = 0;
    long long z;
    
    //find the range
    for (i = 0; i < (ops+1); i++){
        max = offset + sieve[i];
        min = offset + 1;
        offset += sieve[i];
        if (sqrt(size) < max){
            max = sqrt(size);
        }
    }
   
    for(i = min; i < (max+1); i++){
        if(TESTBIT(bitmap, i)){
            //z = i^2
            // all values that are mulitles of z should be marked
            for (z = i * i; z < size; z += i){
                if(TESTBIT(bitmap, z)){
                    CLEARBIT(bitmap, z);
                }
            }
        }
    }
    //http://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
}

//this function is for shared memory objects.
void *mount_shmem(char *path, size_t object_size)
{
    int shmem_fd;
    void *mount;
    
    shmem_fd = shm_open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shmem_fd == -1){
        fprintf(stdout, "Failed to open shared memory object");
        exit(EXIT_FAILURE);
    }
    
    if (ftruncate(shmem_fd, object_size) == -1){
        fprintf(stdout, "Failed to resize shared memory object");
        exit(EXIT_FAILURE);
    }
    
    mount = mmap(NULL, object_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
    
    if (mount == MAP_FAILED){
        fprintf(stdout, "Failed to map shared memory object");
        exit(EXIT_FAILURE);
    }
    
    return mount;
}
//from lecture http://web.engr.oregonstate.edu/~chaneyr/lectures/18-shared_memory.mp4