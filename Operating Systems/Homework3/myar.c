// Pavin Disatapundhu
// disatapp@onid.oregonstate.edu
// CS344-400
// homework#3 Question2

#include <sys/types.h>
#include <unistd.h>
#include <ar.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdint.h>
#include <utime.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>



#define BLOCKSIZE 1024

//check if file exist
int filecheck(char *file){
    struct stat info;
    if (stat(file, &info) != 0){
        return 0;
    }
    return 1;
}


//append a file
void append( int argc, char *argv[]) {
    int file, fd;
    char fname[16];
    struct stat info;
    struct ar_hdr hdr;
    int flag = 0;
    
    if (!(filecheck(argv[2]))){
        printf("%s dose not exist. Creating new Archive.\n",argv[2]);
    } else {
        printf("%s open.\n", argv[2]);
    }
    fd = open(argv[2], O_CREAT | O_APPEND | O_WRONLY, 0666);
    
    if(flag){
        printf("myar: %s: creating\n", argv[2]);
        write(fd, ARMAG, SARMAG);
    }
    printf("myar: %s\n", argv[2]);
    lseek(fd, 0, SEEK_END);
    
    for(int i = 3;i < argc; i++) {
        if ((file = open(argv[i], O_RDONLY)) < 0) {
            printf("myar: %s: No such file or directory\n",argv[i]);
            break;
        }
        printf("myar: testloop %s\n",argv[i]);
        if (fstat(file, &info) == 0) {                strcpy(fname, argv[i]);
            sprintf(hdr.ar_name, "%-11s", strcat(fname,"\0"));
            sprintf(hdr.ar_date, "%-15u", (uint32_t) info.st_mtime);
            sprintf(hdr.ar_uid, "%-10u", (uint32_t) info.st_uid);
            sprintf(hdr.ar_gid, "%-10u", (uint32_t) info.st_gid);
            sprintf(hdr.ar_mode, "%-10o", (uint32_t) info.st_mode);
            sprintf(hdr.ar_size, "%-10u", (uint32_t) info.st_size);
            memcpy(hdr.ar_fmag, ARFMAG, sizeof(hdr.ar_fmag));
            write(fd, (char*) &hdr, sizeof(hdr));
            write(fd, &ARFMAG, 2);
        } else {
            printf("myar: unable to archive %c.\n", fname[i]);
            exit(EXIT_FAILURE);
        }
        char buff[info.st_size];
        int buffering = sizeof(buff);
        read(file, buff, buffering);
        write(fd, buff, buffering);
        
        if (info.st_size % 2) {
            write(fd, "\n", 1);
        }
        close(file);
    }
}


//not finished
void extract(int argc, char *argv[])
{
    int readf, extractf, modef, writef, fdel;
    int size = 0;
    int i = 0;
    int j = 0;
    int length = sizeof(struct ar_hdr);
    char namef[16];
    char bname[16];
    struct ar_hdr hdr[argc];
    struct ar_hdr newhdr;
    argc -= 3;
    int count = argc;
    
    int fd = open(argv[2], O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        printf("Can't open archive.");
        exit(EXIT_FAILURE);
    }
    
    readf = read(fd, (char*) &newhdr, length);
    while (i < count){
        
        if (!strncmp(hdr[i].ar_name, newhdr.ar_name, 16)){
            sscanf(newhdr.ar_name, "%s", namef);
            if (namef[sizeof(namef)-1] == '/')
            {
                namef[sizeof(namef)-1] = 0;
            }
            
            sprintf(namef, "%s", namef);
            sscanf(newhdr.ar_mode, "%o", &modef);
            fdel = creat(namef, modef);
            
            if (fdel == -1) {
                printf("myar: Cannot open file to extract.");
                break;
            }
            readf = 0;
            writef = atoi(newhdr.ar_size);
            while (writef > 0) {
                if(BLOCKSIZE < writef){
                    size = BLOCKSIZE;
                }
                else {
                    size = writef;
                }
                readf = read(fd, bname, size);
                write(fdel, bname, readf);
                writef -= size;
                
                //erase test
                printf("loop error: in here\n");

            }
            //erase test
            printf("loop error: over here\n");
            i++;
            close(fdel);
        } else {
            lseek(fd, atoi(newhdr.ar_size) + (atoi(newhdr.ar_size)%2), SEEK_CUR);
        }
        //erase test
        printf("loop error: out here\n");
    }
}


//needs review
void delete(int argc, char* argv[])
{
    int readf, writef, size, fd, j;
    int length = sizeof(struct ar_hdr);
    char bname[16];
    int copyfile = 0;
    int i = 0;
    struct ar_hdr hdr[argc];
    struct ar_hdr newhdr;
    
    
    int count = argc - 3;
    
    fd = open(argv[2], O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        
        printf("Can't open archive.");
        exit(EXIT_FAILURE);
    }
    
    while ( j < count){
        char file[sizeof(argv[j+2])];
        //create temp
        sprintf(file, "%s.tmp", argv[j]);
        int new_fd = creat(file, 0666);
        if (new_fd == -1) {
            printf("myar: failed to create temp file!");
            exit(EXIT_FAILURE);
        }
        
        lseek(fd, SARMAG, SEEK_SET);
        lseek(new_fd, 0, SEEK_SET);
        write(new_fd, ARMAG, SARMAG);
        
        copyfile = read(fd, (char*) &newhdr, length);
        while ( copyfile == sizeof(struct ar_hdr)) {
            
            if (!strncmp(hdr[i].ar_name, newhdr.ar_name, 16)) {
                lseek(fd, atoi(newhdr.ar_size) + (atoi(newhdr.ar_size)%2), SEEK_CUR);
                i++;
            } else {
                readf = 0;
                size = 0;
                writef = atoi(newhdr.ar_size) + (atoi(newhdr.ar_size)%2);
                write(fd,(char*) &newhdr,length);
                
                while (writef > 0) {
                    if(BLOCKSIZE < writef){
                        size = BLOCKSIZE;
                    }
                    else {
                        size = writef;
                    }
                    readf = read(fd, bname, size);
                    write(new_fd, bname, readf);
                    writef -= size;
                }
            }
        }
        unlink(argv[j+2]);
        link(file, argv[j+2]);
        unlink(file);
        close(new_fd);
    }
}

//needs review

//http://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c

//review
int append(char *self)
    {
}

//int initial(char **argv){
//    int fd = open(argv[2], O_RDWR | O_CREAT, 0666);
//    if (fd == -1) {
//        printf("Can't open archive.");
//        return 0;
//    }
//
//}

                      
int main(int argc, char *argv[]){
    char c;

    if(argc < 3)
    {
        printf("myar: needs more arguments\n");
        exit(EXIT_FAILURE);
    }
    
    if((c = getopt(argc, argv, "qxtvdAw:")) != -1)
    {
        switch(c)
        {
            case 'q':
                printf("q\n");
                append(argc, argv);
            case 'x':
                printf("x\n");
                extract(argc, argv);
//            case 't':
//                printf("t\n");
//                table(argv, 0);
//            case 'v':
//                printf("v\n");
//                table(argv, 1);
            case 'd':
                printf("d\n");
                delete(argc, argv);
            case 'A':
                printf("A\n");
//                A_append()
        }
    }
    return 0;
}