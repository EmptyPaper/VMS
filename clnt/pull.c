#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <unistd.h>
#include <sys/stat.h>
#include <zlib.h>


#include "hash.h"
#include "attr.h"
#include "index.h"
#include "send_recv.h"
#include "updateFile.h"


int conflict=0;


char* getCommitObjectForPull(char* head,int sock){
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
    //can conflict
        fprintf(stderr,"NOTIFICATION: CONFLICT MAY OCCOR\n");
        conflict=1;
    }else if (msg == ERROR){
        dieWithError("push first REPO is empty");
    }
    // sprintf(objectPath,"./.VMS/objects/%c%c%c%c",commitHash[0],commitHash[1],commitHash[2],commitHash[3]);
    // sprintf(objectPath+strlen(objectPath),"/%s",commitHash+4);
    // commit = gzopen(objectPath,"r");
    // for(int i;i<2;i++){
    //     gzgets(commit,buf,128);
    // }

    // strncpy(treeHash,buf+5,OBJECT_NAME_LENGHT);
    // treeHash[OBJECT_NAME_LENGHT]='\0';

    // gzclose(commit);
    fclose(refer);
    return NULL;
    // return treeHash;
    // while(!gzeof(commit)){
    //     gzread(commit, )
    // }
}

void getObjects(int total,int sock){
    int count = 0;
    int size=0;
    int recvByte=0;
    int totalByte=0;
    char buf[64];
    char objectName[65]={0,};
    char objectPath[128]={0,};
    FILE* object;
    unsigned char msg;
    
    while(count < total){
        _recv(sock,objectName,OBJECT_NAME_LENGHT);
        objectName[OBJECT_NAME_LENGHT] = '\0';
        _recv(sock,&size,sizeof(size));
        sprintf(objectPath,"./.VMS/objects/%c%c%c%c",objectName[0],objectName[1],objectName[2],objectName[3]);
        if(access(objectPath,F_OK) == -1){
            mkdir(objectPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        sprintf(objectPath+strlen(objectPath),"/%s",objectName+4);
        objectPath[80]='\0';
        // sprintf(objectPath,"%s/objects/%s",repoPath,objectName);
        object = fopen(objectPath,"wb");
        if(object == NULL){
            fprintf(stderr,"\n%s\n",objectPath);
            perror("fopen");
            exit(1);
        }
        while(totalByte < size){
            recvByte = _recv(sock,buf,64);
            fwrite(buf,1,recvByte,object);
            totalByte+=recvByte;
        }
        count++;
        msg=PUSH_REQ;
        _send(sock,&msg,1);
        fprintf(stderr,"%s is receved %d \n",objectName,total);
        recvByte = 0;
        totalByte = 0;
        fclose(object);
    }
}
// void updateFiles(char* treeHash){
//     char* objectPath;
//     gzFile object;
//     char buf[256];
//     int instanceLen;
//     static int count=1;
//     char* blank;
//     int offset;
//     indexContent* content;
//     char name[256];

//     objectPath = hashToPath(treeHash);
    
//     object = gzopen(objectPath,"r");
//     if(object==NULL){
//         perror("gzopen");
//         exit(1);
//     }
//     while(!gzeof(object)){
//         gzgets(object,buf,256);
//         blank = strrchr(buf,' ');
//         offset = blank - buf; 
//         strncpy(name,buf+4)
//         content = malloc(sizeof(indexContent));
//         strncpy(content->name,buf+offset+1,64);
//         content->name[64]='\0';
//         red_black_insert(content,1);
//         if(!strncmp(buf,"tree",4)){
//             objectFactory(content->name);
//         }
//         count++;
//     }
//     gzclose(object);
//     return count;
// }

void logUpdate(char* head,char* commitHash){
    char logPath[256]={0,};
    char headPath[256] = {0,};
    char line[256];
    sprintf(logPath,"./.VMS/log/heads/");
    sprintf(logPath+strlen(logPath),"%s",head);
    char parentCommit[65]={0,};

    FILE *log = fopen(logPath,"r+");
    if(log == NULL){
        log = fopen(logPath,"w");
        for(int i=0;i<64;i++){
            parentCommit[i]='0';
        }
        fprintf(log,"%s %s <--PULLED",parentCommit,commitHash);
        fclose(log);
        return;
    }else{
        while(!feof(log)){
        fgets(line,256,log);
        fprintf(stderr,"log job");
        }
        strncpy(parentCommit,line+65,COMMIT_HASH_LENGHT);
        parentCommit[64]='\0';
        
        fprintf(log,"\n%s %s",parentCommit,commitHash);
        // fgets(line,256,fp);
        // while(!feof(fp)){
        //     fgets(line,256,fp);
            
        // }
        fprintf(stderr,"log job");
        fclose(log);
    }

    sprintf(headPath,"./.VMS/log/head/refers/heads");
    sprintf(headPath+strlen(headPath),"/%s",head);
    FILE* HEAD = fopen(headPath,"w");
    fprintf(HEAD,"%s",commitHash);
    fclose(HEAD);
    
    // char* commitPath = hashToPath(commitHash);
    // FILE COMMIT = fopen(commitPath,"r");
    // char buf[128];
    // for(int i=0;i<2;i++){
    //     gzgets(COMMIT,buf,128);
    // }
    // b

}

void pullCmd(int sock,char* head){
    unsigned char msg;
    char treeHash[65];
    char commitHash[65];
    int total;

    NILL = malloc(sizeof(contents));
    NILL->color = BLACK;
    ROOT = NILL;

    msg = PULL_REQ;
    _send(sock,&msg,1);
    

    pushRepoHash(sock);

    _recv(sock,&msg,1);
    if(msg == ERROR){ //if repo is not exixst
        dieWithError("REPO NOT EXIST");
    }else if(msg == PULL_REQ ){
        fprintf(stderr," -> REPO EXIST\n");
    }
    _send(sock,head,HEAD_NAME_LENGHT);

    getCommitObjectForPull(head,sock);
    loadIndex();
    //cal distance
    _recv(sock,&total,sizeof(total));
    getObjects(total,sock);
    _recv(sock,commitHash,OBJECT_NAME_LENGHT);
    commitHash[OBJECT_NAME_LENGHT]='\0';
    _recv(sock,treeHash,OBJECT_NAME_LENGHT);
    treeHash[OBJECT_NAME_LENGHT]='\0';
    logUpdate(head,commitHash);
    updateFiles(treeHash);

}