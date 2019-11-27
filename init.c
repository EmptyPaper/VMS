#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
void DieWithError(char*);
int init();

int main(){
    init();
    return 0;
}

int init(){
    struct dirent* ent;
    DIR *dir = opendir("./");
    FILE *fp;

    if(dir!=NULL){
        while((ent = readdir(dir)) != NULL){
            if(!strcmp(".VMS", ent->d_name)){
                rmdir("./.VMS");
                fprintf(stderr,"INITIATING repository");
            }
        }
    }
    else{
        DieWithError("open Dir Error");
    }
    
    mkdir("./.VMS", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("./.VMS/vetors", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("./.VMS/codes", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("./.VMS/objects", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    

    fp = fopen("./HEAD","w");
    if(fp == NULL){
        DieWithError("HEAD failed");
    }
    fprintf(fp,"ref : master");
    fclose(fp);

    return 0;

}