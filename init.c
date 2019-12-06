#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "genKey.h"
int init();

int init(){
    struct dirent* ent;
    DIR *dir = opendir("./");
    FILE *fp;

    if(dir!=NULL){
        while((ent = readdir(dir)) != NULL){
            if(!strcmp(".VMS", ent->d_name)){
                rmdir("./.VMS");
                fprintf(stderr,"NOTIFICATION : INITIATING repository\n");
            }
        }
    }
    else{
        perror("open Dir Error");
    }
    
    mkdir("./.VMS", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("./.VMS/refers", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("./.VMS/refers/heads", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("./.VMS/objects", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("./.VMS/log", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("./.VMS/log/heads", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


    fp = fopen("./.VMS/HEAD","w");
    if(fp == NULL){
        perror("HEAD failed");
    }
    fprintf(fp,"ref : master");
    fclose(fp);

    return 0;

}

int main(){
    char email[100];
    char nick[100];
    FILE* fp;
    init();
    if(access("./user.key",F_OK) != -1){
        /* PK.key is exist */
    }else{
        genPrivateKey();
        fprintf(stdout,"NOTIFICATION : %s","user.key is generated!\n");
    }
    fprintf(stdout,"ENTER EMAIL -> ");
    fgets(email,100,stdin);

    fprintf(stdout,"NICK -> ");
    fgets(nick,100,stdin);
    
    fp = fopen("./.VMS/info","w");
    fwrite("EMAIL:",1,6,fp);
    fwrite(email,1,strlen(email),fp);
    fwrite("NICK:",1,5,fp);
    fwrite(nick,1,strlen(nick),fp);
    fwrite("REPO:",1,5,fp);
    
    fclose(fp);
    
    
    return 0;
}
