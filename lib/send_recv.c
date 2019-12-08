#include <sys/socket.h> 
#include <stdio.h>
#include "attr.h"

void _send(int sock, void* item, int size){
    if(send(sock,item,size,0) < size)
        dieWithError("send few data than expected");
}
int _recv(int sock, void* item, int size){
    int recvByte;
    if((recvByte = recv(sock,item,size,0)) < 0)
        dieWithError("recv few data than expected");
    return recvByte;
}


