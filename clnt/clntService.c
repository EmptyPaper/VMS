#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "attr.h"
#include "send_recv.h"


void clntService(int argc,char*argv[]){
    int sock;
    struct sockaddr_in servAddr;
    unsigned short servPort = SERV_PORT;
    char* sig;
    char servIp[16] = SERV_IP;

    if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        dieWithError("socket() faield");
    
    servAddr.sin_family = AF_INET;
    memset(&servAddr,0,sizeof(servAddr));
    servAddr.sin_addr.s_addr = inet_addr(servIp);
    servAddr.sin_port = htons(servPort);
    if(connect(sock,(struct sockaddr*) &servAddr, sizeof(servAddr))<0)
        dieWithError("connet() failed");

    if(argc==2){
        if(!strcmp(argv[1],"push"))
            pushCmd(sock);
    }
    if(argc==3){
        if(!strcmp(argv[1],"pull"))
            pullCmd(sock,argv[2]);
    }


    close(sock);
}

int main(int argc,char* argv[]){
    clntService(argc,argv);
}

