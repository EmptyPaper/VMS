#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>

#include "attr.h"
#include "send_recv.h"


void handleClnt(int sock){
    unsigned char msg;
    unsigned char mask = 0xff;
    unsigned char opt;
    _recv(sock,&msg,1);
    opt = mask & msg;
    switch (opt)
    {
    case 1:
        createRepository(sock);
        break;
    // case 2:
    //     push(sock);
    //     break;
    // case 4:
    //     pull(sock);
    //     break;
    // case 8:
    //     Log(sock);
    //     break;
    default:
        dieWithError("NOTIFICATION:WRONG MSG TYPE");
        break;
    }
    close(sock);
}