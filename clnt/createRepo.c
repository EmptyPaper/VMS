#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <unistd.h>
// #include <pthread.h>
#include "hash.h"
#include "genKey.h"
#include "attr.h"
#include "send_recv.h"

typedef struct _fileType{
    int size;
    char type[5];
    char name [256];
}fileType;


void pushInfo(int sock,FILE* info){
    char line[256];
    char nick[NICK_LENGHT];
    char email[EMAIL_LENGHT];
    
    fgets(line,256,info);
    strncpy(email,line+6,EMAIL_LENGHT);
    email[EMAIL_LENGHT-1] ='\0';

    fgets(line,256,info);
    strncpy(nick,line+5,NICK_LENGHT);
    nick[NICK_LENGHT=1] = '\0';

    _send(sock,email,EMAIL_LENGHT);
    _send(sock,nick,NICK_LENGHT);
}

void pushPublicKey(int sock){
    char* publicKey;
    publicKey = genPrivateKey();
    _send(sock,publicKey,strlen(publicKey));
}
void updateInfo(char* hash,FILE* info){
    fseek(info,0,SEEK_END);
    fprintf(info,"%s",hash);
    fclose(info);
}

void createRepo(int sock){
    unsigned char msg;
    char repoName[REPO_NAME_LENGHT];
    char repoHash[REPO_HASH_LENGH+1];

    msg = CREATE_REPO;
    FILE* info;

    fpirntf(stdout,"NOTIFICATION:CONNECTING SERV\n");
    if(send(sock,&msg,1,0) < 1){
        dieWithError("createRepo msg Send failed");
    }
    if(recv(sock,&msg,1,0) < 1){
        dieWithError("CreateRepo msg recv failed");
    }
    fprintf(stdout,"NOTIFICATION:CONNECTED......\n");
    pushPublicKey(sock);

    _recv(sock,&msg,1);
    if(msg==0){
        dieWithError("NOTIFICATION:Already Repository have SET\n");
    }
    fprintf(stdout,"Enter REPO-NAME -> ");
    fgets(repoName,REPO_NAME_LENGHT-1,stdin);
    repoName[strlen(repoName)-1] ='\0';

    info =fopen("./.VMS/info","r+");
    if(info == NULL)
        dieWithError(".VMS interrupted");
    pushInfo(sock,info);

    if(recv(sock,repoHash,sizeof(repoHash),0) < sizeof(repoHash)){
        dieWithError("recved few msg than expected");
    }
    updateInfo(repoHash,info);
}

void createRepoCmd(int argc,char* argvp[],int sock){
    if(access("./.VMS/",F_OK) == -1){
        dieWithError("INITIATE VMS FIRST!\n");
    }
    createRepo(sock);
}

int  main(int argc,char* argv[]){
    createRepoCmd(argc,argv);
}