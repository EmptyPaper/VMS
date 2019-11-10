#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef FILE_NAME_SIZE
    define FILE_NAME_SIZE 64
#endif

struct fileInfo{
    unsigned int size;
    char name[30];
};

typedef struct _node{
    node n;
    void* d;
}node;

void DieWithError(char * errorMessage) {
    perror(errorMessage);
    exit(1);
}
unsigned int checkRepository(){
    if(access("./.VMS", F_OK) != -1){
        return 1;
    }
    else{
        DieWithError("VMS is not initiated");
    }
    return 0;
}

unsigned int checkFileSize(FILE* fp){
    unsigned int fileSize;
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return fileSize;
}

void* hashSha256(void * data,FILE *fp);

