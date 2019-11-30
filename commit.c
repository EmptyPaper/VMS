#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "ds.h"

unsigned int checkRepository();

void checkIndex(){
    FILE* INDEX;
    if(access("./.VMS/index", F_OK) != -1){
        struct fileInfo* info;
        INDEX = fopen();
    }
    else{
        DieWithError("Empty Index");
    }
}

void DieWithError(char*);

void* hashSha256(void * data);

void vector(){

}

void hashChain(){
    FILE *HEAD;
    char buffer[256];
    char code_name[64];
    if(checkRepository()){
        HEAD = fopen("./.VMS", "r");
        if(HEAD == NULL){
            DieWithError("VMS FILE IS CONTAMINATED");
        }
        fgets(buffer,256, HEAD);
        if(strcmp(buffer,"ref : master") != 0){
            fprintf(stderr,"first commit");
        }
        else{
            strcpy(code_name,buffer+13);
            
        }
    }
    else{
        DieWithError("VMS is not initiated");
    }

}
void checkAdd(){
    struct FileInfo fInofo;
    FILE *index;
    
}
void commit(int argc,char* argvp[]){
    
}


unsigned char* addTree(hashNode* hashs, int objNum){
    hashNode* temp;
    temp = hashs;
    unsigned char* treeHash;
    char* treeobj = (char*)malloc((sizeof(char))*objNum<<8); //* 32
    char dir[100]; //Just apporxiamted
    char path[100];

    sprintf(treeobj,"Tree %d%c\nblob ",objNum,'\0');

    while(temp!=NULL){
        sprintf(treeobj,"\n");
        snprintf(treeobj,32,"blob %s\0 %s",temp->name,temp->hash);
        temp = temp->next;
    }
    treeHash = hashchars(treeobj);

    sprintf(dir,"./.VMS/objects/%02x%02x",treeHash[0],treeHash[1]);
    sprintf(path,"%s/",dir);
    for(int i=2; i < 32; i++){                // Hash to char*
            sprintf(path, "%02x",treeHash[i]); 
    }
    if(checkfile(dir)){     //first two char of hash dir is exixted
        fprintf(stderr,"%s -> ");
        if(checkFile(path)){  //Object is Existed
                fprintf(stderr,"%s\nObject Existed\n",path+19);
                return treeHash;
        }
    }else{
        mkdir(dir, "S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH");
    }
    createTreeObject(treeobj,dir);
    updateIndex(treeHash,hashs);
    return hashToString(treeHash);
}
void createTreeObject(char* contents,char* objectName){
    gzFile* object;
    int readByte;

    object = gzopen(objectName,"wb");
    if(object==NULL)
        perror("gzwrite error");

    if(gzwrite(object,contents,sizeof(contents)) < 0)
        perror("gzwrite error");

    gzclose(object);
    fprintf(stderr,"Tree object CREATED -> %s",objectName);
}