#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <unistd.h>
#include <zlib.h>


#include "hash.h"
#include "attr.h"
#include "index.h"
#include "send_recv.h"

/* get head info 
-> cat-file commit data
-> push commit data     ->if commit data exist already up-to-date
                        ->else new commitdata
                        find co parrent commit
                        and tell distance
                        distance == 1 -> push
                        or 0 -> push
-> cat-file root-tree 
-> factoring object
-> push
*/
//head


char* getHead(int sock){
    FILE* HEAD;
    char* head = malloc(sizeof(char)*(HEAD_NAME_LENGHT+1));
    HEAD = fopen("./.VMS/HEAD","r");
    fseek(HEAD,6,SEEK_SET);
    fread(head,1,HEAD_NAME_LENGHT,HEAD);
    head[strlen(head)]='\0';
    _send(sock,head,HEAD_NAME_LENGHT+1);//4
    return head;
}
char* getCommitObjectForPush(char* head,int sock){
    gzFile commit;
    FILE* refer;
    unsigned char msg=0;
    int distance = 0;
    char* commitHash = malloc(sizeof(char)*(COMMIT_HASH_LENGHT+1));
    char* treeHash = malloc(sizeof(char)*(OBJECT_NAME_LENGHT+1));
    char referPath[100]={0,};
    char objectPath[100]={0,};
    char line[256]={0,};
    char buf[128];

    sprintf(referPath, "./.VMS/refers/heads/%s",head);
    
    refer = fopen(referPath,"r");
    fread(commitHash,1,COMMIT_HASH_LENGHT,refer);
    commitHash[COMMIT_HASH_LENGHT] ='\0';


    indexContent* content = malloc(sizeof(indexContent));
    strncpy(content->name,commitHash,64);
    content->name[64]='\0';
    red_black_insert(content);
    
    _send(sock,commitHash,COMMIT_HASH_LENGHT);
    _recv(sock,&msg,1);
    if(msg==UP_TO_DATE){ //distance 0;
        dieWithError("NOTIFICATION:Commit already up-to-date");
    }else if (msg == MORE_DATA){ //distacne 2;
        _recv(sock,&distance,sizeof(distance));
        fprintf(stderr,"NOTIFICATION: %d - DISTANCE BEHIND FROM REMOTE PULL FIRST\n",distance);
        exit(1);
    }
    sprintf(objectPath,"./.VMS/objects/%c%c%c%c",commitHash[0],commitHash[1],commitHash[2],commitHash[3]);
    sprintf(objectPath+strlen(objectPath),"/%s",commitHash+4);
    commit = gzopen(objectPath,"r");
    for(int i=0;i<2;i++){
        gzgets(commit,buf,128);
    }

    strncpy(treeHash,buf+5,OBJECT_NAME_LENGHT);
    treeHash[OBJECT_NAME_LENGHT]='\0';

    content = malloc(sizeof(indexContent));
    strncpy(content->name,treeHash,64);
    content->name[64]='\0';
    red_black_insert(content);

    gzclose(commit);
    fclose(refer);

    return treeHash;
    // while(!gzeof(commit)){
    //     gzread(commit, )
    // }
}
int objectFactory(char* treeHash){
    char* objectPath;
    gzFile object;
    char buf[256];
    int instanceLen;
    static int count=2;
    char* blank;
    int offset;
    indexContent* content;

    objectPath = hashToPath(treeHash);
    
    object = gzopen(objectPath,"r");
    if(object==NULL){
        perror("gzopen");
        exit(1);
        
    }
    while(!gzeof(object)){
        gzgets(object,buf,256);
        blank = strrchr(buf,' ');
        offset = blank - buf; 

        // instanceLen = strlen(buf);
        content = malloc(sizeof(indexContent));
        strncpy(content->name,buf+offset+1,64);
        content->name[64]='\0';
        red_black_insert(content);
        if(!strncmp(buf,"tree",4)){
            objectFactory(content->name);
        }
        count++;
    }
    gzclose(object);
    return count;
}



void pushObject(contents* objects,int total,int sock){
    int size;
    char* path;
    FILE* fp;
    char buf[64];
    static int count = 0;
    int byteRecv = 0;
    int indicate=0;
    unsigned char msg;
    if(objects!=NILL){
        pushObject(objects->left,total,sock);
        path = hashToPath(objects->content->name);
        fp = fopen(path, "rb");
        if(fp == NULL){
            perror("fp");
            fprintf(stderr,"\n%s\n",objects->content->name);
            exit(1);
        }
        fseek(fp,0,SEEK_END);
        size=ftell(fp);
        _send(sock,objects->content->name,OBJECT_NAME_LENGHT);
        _send(sock,&size,sizeof(size));
        fseek(fp,0,SEEK_SET);
        while(!feof(fp)){
            indicate+=byteRecv = fread(buf,1,SIGBUF,fp);
            _send(sock,buf,byteRecv);
            fprintf(stdout,"\rPushing OBJ(%4d/%4d) %4d/%4d",count,total,indicate,size);
            if(indicate%4==1)
                fprintf(stdout,"|");
            else if(indicate%4==2)
                fprintf(stdout,"/");
            else if(indicate%4==3)
                fprintf(stdout,"-");
            else if(indicate%4==0)
                fprintf(stdout,"\\");
            fflush(stdout);
        }
        _recv(sock,&msg,1);
        count++;
        fprintf(stdout,"\rPushing OBJ(%4d/%4d) %4d/%4d",count,total,indicate,size);
        if(msg!=PUSH_REQ){
            dieWithError("object push error");
        }
        fclose(fp);
        pushObject(objects->right,total,sock);
        free(path);
    }
    
    
}

// void pushLog(char* head,int sock){
//     FILE* log;
//     char logPath[100]={0,};
//     int size;

//     sprintf(logPath,"./.VMS/log/heads/%s",head);
//     log = fopen(logPath,"rb");
//     fseek(log,0,SEEK_END);
//     size = ftell(log);
//     fseek(log,0,SEEK_SET);

//     while(feof(log)){
        
//     }

// }

void pushCmd(int sock){
    NILL = (contents*)malloc(sizeof(contents));
    NILL->color =BLACK;
    ROOT = NILL;
    int count;
    unsigned char msg=0;
    char* head;
    char* treeHash;

    msg=PUSH_REQ;
    _send(sock,&msg,1);

    /* send repo info*/
    pushRepoHash(sock);//1
    pushPublicKey(sock);//2

    //perm check
    _recv(sock,&msg,1);//3
    if(msg == ERROR){
        dieWithError("NOT PERMITED");
    }else if(msg == PUSH_REQ ){
        fprintf(stderr," -> PERMITED\n");
    }

    head = getHead(sock);//4
    treeHash = getCommitObjectForPush(head,sock);//5
    count = objectFactory(treeHash);
    _send(sock,&count,sizeof(int));
    pushObject(ROOT,count,sock);

    free(head);
    free(treeHash);
}