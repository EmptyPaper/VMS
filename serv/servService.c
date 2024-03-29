#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <pthread.h>


#include "attr.h"
#include "send_recv.h"

#define MAXPENDING 5

void handleClnt(int sock);


pthread_t tid;

typedef struct _user{
    char nick[NICK_LENGHT];
    char email[EMAIL_LENGHT];
    char* publicKey;
}user;

void* service(void* clntSock){
    int sock = (int)clntSock;
    handleClnt(sock);
    /* -> handle client socket */
}

int main(int argc,char* argv[]){
    int servSock;
    int clntSock;
    struct sockaddr_in servAddr;
    struct sockaddr_in clntAddr;
    unsigned short servPort = SERV_PORT;
    unsigned int clntLen;
    int option = 1;

    if((servSock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
        dieWithError("socket() failed");
    
    setsockopt(servSock,SOL_SOCKET,SO_REUSEADDR,&option,sizeof(option));

    memset(&servAddr,0,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(servPort);

    if(bind(servSock,(struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
        dieWithError("binding failed");
    if(listen(servSock,MAXPENDING) < 0)
        dieWithError("listend failed");

    while(1){
        clntLen = sizeof(clntAddr);
        if((clntSock = accept(servSock,(struct sockaddr*)&clntAddr,
        &clntLen))<0)
            dieWithError("accept() failed");
        if(pthread_create(&tid,NULL,service,(void*)clntSock) < 0)
            dieWithError("thread creation failed");
        else{
            printf("Client ip : %s\n",inet_ntoa(clntAddr.sin_addr));
            printf("port : %d\n",(int)clntAddr.sin_port);
            fprintf(stdout,"----------------------------------------------\n");
            /* add more info here */
        } 
    }
}

