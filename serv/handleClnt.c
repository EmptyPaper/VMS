#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>

#include "attr.h"


void _send(int sock, void* item, int size){
    if(send(sock,item,size,0) < size)
        dieWithError("send few data than expected");
}
void _recv(int sock, void* item, int size){
    int recvByte;
    if((recvByte = recv(sock,item,size,0)) < size)
        dieWithError("recv few data than expected");
}

void handleClnt(int sock){
    int flag = -1;
    unsigned char msg;
    unsigned char mask = 0xff;
    unsigned char opt;
    while(flag!=0){
        _recv(sock,&msg,1);
        opt = mask & msg;
        switch (opt)
        {
        case 1:
            createRepository();
            break;
        case 2:
            push();
            break;
        case 4:
            pull();
            break;
        case 8:
            Log();
            break;
        case 0xff:
            flag=0;
            break;
        default:
            dieWithError("NOTIFICATION:WRONG MSG TYPE");
            break;
        }
    }
    close(sock) 
}