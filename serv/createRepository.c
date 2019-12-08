#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include "attr.h"
#include "send_recv.h"
#include "genKey.h"
#include "hash.h"


char* getPublicKey(int sock){
    int sigSize;
    int msg;
    int recvByte=0;
    char* publicKey;
    unsigned char* digest;
    char* hash;
    char* path;
    FILE* perm;

    path = malloc(sizeof(char)*256);

    _recv(sock,&sigSize,sizeof(sigSize));
    publicKey = malloc(sigSize);
    while(recvByte < sigSize){
        recvByte+=_recv(sock,publicKey+recvByte,SIGBUF);
    }
    digest = hashchars(publicKey);
    hash = hashToString(digest);
    sprintf(path,"./%c%c%c%c",hash[0],hash[1],hash[2],hash[3]);
    if(access(path,F_OK) != -1){
        sprintf(path+strlen(path),"/%s",hash+4);
        if(access(path,F_OK) != -1){
            msg=0;
            _send(sock,&msg,1);
            free(digest);
            return NULL;
        }else{
            mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
    }else{
        mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        sprintf(path+strlen(path),"/%s",hash+4);
        mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    msg=CREATE_REPO;
    _send(sock,&msg,1);
    _send(sock,hash,64);
    fprintf(stderr,"LOG:REPO hash is sended\n");
    perm = fopen(hash,"w");
    fprintf(perm,"%s",hash);

    fclose(perm);
    free(digest);
    return path;
}
void init(char* path){
    char refers[100]={0,};
    char objects[100]={0,};
    char log[100]={0,};
    sprintf(refers,"%s/refers",path);
    mkdir(refers, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    sprintf(refers+strlen(refers),"/heads");
    mkdir(refers, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    sprintf(objects,"%s/objects",path);
    mkdir(objects, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    sprintf(log,"%s/log",path);
    mkdir(log, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    sprintf(log+strlen(log),"/heads");
    mkdir(log, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}
void createRepository(int sock){
    unsigned char msg;
    int sigSize;
    char* path;
    char buf[SIGBUF];
    msg = CREATE_REPO;

    _send(sock,&msg,1); //oh 
    path = getPublicKey(sock);
    if(path==NULL){
        fprintf(stderr,"duplicated REPO");
        return;
    }
    init(path);
    
}