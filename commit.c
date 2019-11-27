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