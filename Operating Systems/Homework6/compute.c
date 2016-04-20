//name: Pavin Disatapundhu
//e-mail: disatapp@onid.oregonstate.edu
//class: CS344-400
//assignment: Homework #6, compute.c


#define _XOPEN_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

//from lecture
#define PORT 1234
#define IP_ADDR "127.0.0.1"
#define ARRSIZE 6
#define MAXSIZE 8192
#define MAXSOCKADDR 128


static void sighandle(int sig);
void compute(int sockfd, int min, int max);
void xmlparse(char *msend, char *proc);
int socketconnect(char *proc);


int main(int argc, char **argv){
    int j = 10000000;
    int i, k;
    int min;
    int max;
    int cpu;
    char strrecv1[MAXSIZE];
    char datarecv1[MAXSIZE];
    char *proc1 = malloc(sizeof(char) * 10);
    char *proc2 = malloc(sizeof(char) * 10);
    char strsnd[MAXSIZE];
    char strrecv2[MAXSIZE];
    char datarecv2[MAXSIZE];
    double totaltime;
    
    //sockest ands signals
    int socket1, socket2;
    struct sigaction sig;
    struct sigaction sigp;
    clock_t starttime, stoptime;
    pid_t control; //fork
    sig.sa_handler = sighandle;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;
    sigaction(SIGINT, &sig, NULL);
    sigaction(SIGHUP, &sig, NULL);
    sigaction(SIGQUIT, &sig, NULL);
    //http://www.gnu.org/software/libc/manual/html_node/Sigaction-Function-Example.html
    sigp.sa_handler = sighandle;
    sigemptyset(&sigp.sa_mask);
    sigp.sa_flags = 0;
    sigaction(SIGPIPE, &sigp, NULL);
    
    //signal process
    switch(control = fork()){
        case 0:
            
            //connect and send
            sprintf(proc1, "%s","-h");
            socket1 = socketconnect(proc1);
            while(1){
                if(recv(socket1, strrecv1, MAXSIZE,0) == 0){
                    perror("Fork Monitor:");
                    exit(EXIT_FAILURE);
                }
                sscanf(strrecv1, "<?xml version='1.0' encoding='UTF-8'?><server><action>%[^<]", datarecv1);
                //http://en.wikipedia.org/wiki/Regular_expression
                if (!strcmp(datarecv1, "-k")){
                    printf("Exiting program\n");
                    kill(getppid(), SIGQUIT);
                    close(socket1);
                    exit(EXIT_FAILURE);
                }
                memset(strrecv1,0, MAXSIZE);
            }
            break;
        case -1:
            perror("Child failed");
            exit(EXIT_FAILURE);
            break;
        default:
            break;
    }
    //create socket
    sprintf(proc2, "%s","-c");
    socket2 = socketconnect(proc2);
    if (recv(socket2, strrecv2, MAXSIZE,0) == 0){
        perror("Fail to Receive:");
        exit(EXIT_FAILURE);
    }
    //http://www.beej.us/guide/bgnet/output/html/multipage/recvman.html
    sscanf(strrecv2, "<?xml version='1.0' encoding='UTF-8'?><server><message>%[^<]", datarecv2);
    //http://www.cplusplus.com/reference/cstdio/sscanf/
    printf("%s\n", datarecv2);
    memset(strrecv2, 0, MAXSIZE);
    

    //compute flops/iops and save
    starttime = clock();
    for(i = 1; i < j; i++){
        k = i % 20;
    }
    stoptime = clock();
    //calculate time to calculate
    totaltime = ((double) (stoptime - starttime)) / CLOCKS_PER_SEC;
    printf("Time %f sec\n",totaltime);
    cpu = (int)(j / totaltime);
    printf("Calc per Sec: %d\n",cpu);
    
    //send cpu timing and recv max & min
    while(1){
       
        //send timing to server
        sprintf(strsnd, "<?xml version='1.0' encoding='UTF-8'?><query><action>-r</action><cpu>%d</cpu></query>", cpu);
        send(socket2, strsnd, strlen(strsnd), 0);
         printf("Send\n");
        //get range of the perfect numbers from server
        if (recv(socket2, strrecv2, MAXSIZE,0) == 0){
            perror("Recv() error:");
            break;
        }
        printf("recieved\n");
        //datarecv2 from server
        sscanf(strrecv2, "<?xml version='1.0' encoding='UTF-8'?><range><min>%d</min><max>%d</max></range>", &min, &max);
        printf("The range is %d - %d.\n", min, max);
        //send the
        compute(socket2, min, max);
        memset(strrecv2, 0, MAXSIZE);
    }
    
    //kill process, and close sockets
    close(socket2);
    kill(control, SIGKILL);
    wait(NULL);
    return 0;
}
static void sighandle(int sig){
    wait(NULL);
    exit(EXIT_FAILURE);
}

//compute the perfect number
void compute(int sockfd, int min, int max){
    int sum, num;
    int x = 0;
    int arr[ARRSIZE];
    char strsnd[MAXSIZE];
    char data[50];
    char end[30];
    double totaltime;
    
    clock_t starttime, stoptime;
    printf("Initilizing\n\n");
    for (int i = 0; i < ARRSIZE; i++){
        arr[i] = 0;
    }
    printf("Computing\n");

    strcpy(end,"</perfect></results></query>");
    starttime = clock();
    for(num = min; num <= max; num++){
        sum = 0;
        for(int j = 1; j < num; j++){
            if(!(num%j)){
                sum+=j;
            }
        }
        if(sum == num){
            printf("%d\n",num);
            arr[x] = num;
            x++;
        }
    }
    printf("Finished\n");
    //http://stackoverflow.com/questions/4554351/perfect-number-in-c
    stoptime = clock();
    totaltime = ((double) (stoptime - starttime)) / CLOCKS_PER_SEC;
    printf("Computing time: %f\n", totaltime);
    //format string
    sprintf(strsnd, "<?xml version='1.0' encoding='UTF-8'?><query><action>-rn</action><results><curmax>%d</curmax><perfect>",max);
    for (int i = 0; i < x; i++){
        sprintf(data,"<pn>%d</pn>",arr[i]);
        strcat(strsnd,data);
    }
    strcat(strsnd,end);
    //https://piazza.com/class/hyeuflquukj3r4?cid=281
    
    send(sockfd, strsnd, strlen(strsnd),0);
    memset(strsnd,0,MAXSIZE);
}

//parser this xml
void xmlparse(char *msend, char *proc){
    sprintf(msend, "<?xml version='1.0' encoding='UTF-8'?><query><action>%s</action></query>", proc);
    //http://www.w3schools.com/xml/
}

int socketconnect(char *proc){
    struct sockaddr_in maddr;
    int msockfd;
    char msend[MAXSIZE];
    
    msockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&maddr, 0, sizeof(maddr));
    maddr.sin_family = AF_INET;
    maddr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP_ADDR, &maddr.sin_addr);
    //http://www.beej.us/guide/bgnet/output/html/multipage/inet_ntopman.html
    
    //connect to avalible socket
    connect(msockfd,(struct sockaddr *)&maddr, sizeof(maddr));

    //xml parse
    xmlparse(msend, proc);
    send(msockfd, msend, strlen(msend), 0);
    
    return msockfd;
}
