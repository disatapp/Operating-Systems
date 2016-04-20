int i;
int maxi;
int connfd;
int nready;
int clilent[FD_SETSIZE];

int listenfd;
int socketfd;

ssize_t n;
char buf[MAXLINE];
socklen_t clilen;
struct sockaddr_in cliaddr;
struct sockaddr_in serveraddr;

fd_set rset;
fd_set allset;


//from lecture
maxfd = listenfd;
maxi = -1;

for (int i = 0; i < FD_SETSIZE; i++){
    clilent[i] = -1;
}

FD_ZERO(&allset);
FD_SET(listenfd, &allset);

listenfd = socket(AF_INET, SOCK_STREAM, 0);

bzero(&servaddr, sizeof(servaddr));
servaddr.sin_family = AF_INET;
servaddr.sin_port = htons(PORTNUM);
servaddr.sinaddr.s_addr = htonl(INADDR_ANY);

bind(listenfd,(struct sockaddr *)&servaddr, sizeof(servaddr));
listen(listenfd, LISTENQ);

for(;;){
    rset = allset;//structure assignment is a full shallow copy
    nready = select(maxfd + 1, &rset, NULL,NULL,NULL);
    if(nready == -1){
        perror("Select didnt work");
        exit(EXIT_FAILURE);
    }
    
    
    if(FD_ISSET(listenfd, &rset)){
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr,&clilen);
        
        for (int i = 0; i < FD_SETSIZE; i++){
            if(client[i] <0){
                clilent[i] = connfd;
                break;
            }
        }
        if(i == FD_SETSIZE){
            perror("too many clients");
            exit(EXIT_FAILURE);
        }
        
        FD_SET(connfd, &allset);
        
        if(connfd > maxfd){
            maxfd = connfd;
        }
        if (i > maxi){
            maxi = i;
        }
        if(--nready <= 0){
            continue; //back to top of loop
        }
        
    }
    
    for(i = 0; i <=maxi;++i){
        if((socketfd = client[i]) < 0){
            continue;
        }
        if(FD_ISSET(socketfd, &rset)){
            bzero(buf, MAXLINE);
            if((n = read(socketfd, buf, MAXLINE)) == 0){
                close(socketfd);
                break;
            }
            else{
                fputs(buf, stdout);
                write(sockfd, buf, n);
            }
            if(--nready <= 0){
                break; //back to top of loop
            }
        }
    }
}
//http://web.engr.oregonstate.edu/~chaneyr/lectures/23-sockets_select.mp4
