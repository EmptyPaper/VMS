#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stat.h>
#include <zlib.h>

#include <openssl/sha.h>

#include "ds.h"

// void create_dir(){
//     #ifdef __linux__
//         mkdir(".vms",777);
//     #else
//         _mkdir(".vms");
//     #endif
// }


unsigned char* addFile(FILE* fp,const char* path,Node* root){
    Node* s;
    struct fileInfo* index;
    struct fileInfo fi;
    unsigned char *hash;
    char dir[100]; //Just apporxiamted
    char path[100];

    FILE *object;

    hash = hashFile(path);
    sprintf(dir,"./.VMS/objects/%02x%02x",hash[0],hash[1]);
    sprintf(path,"%s/",dir);
    for(int i=2; i< 32; i++){                // Hash to char*
            sprintf(path, "%02x",hash[i]); 
    }
    if(checkfile(dir)){     //first two char of hash dir is exixted
        fprintf(stderr,"%s -> ");
        if(checkFile(path)){  //Object is Existed
                fprintf(stderr,"%s\nObject Existed\n",path+19);
                return hash;
        }
    }else  //make object;
        mkdir(dir, "S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH");
    object = fopenn(path,"rb");
    
    memset(&fi,0,sizeof(struct fileInfo));
    strcpy(fi.name,path);

    // s = search(root,path);
    // if(s != NULL){
    //     index = s->d;
    //     memcpy(fi.repo,index->repo,32);
    //     memcpy(fi.stage,index->stage,32);
    //     memcpy(fi.size,index->size,sizeof(int));
    // }
    // fwrite(&fi,sizeof(struct fileInfo),1,fp);

    return hash;
}


void addDir(Node* n,const char* path,SHA256_CTX* ctx){
    DIR *dir = opendir(path);
    FILE* componet;

    struct fileInfo fi;
    struct dirent ent;

    if(isfile(path)){
        closedir(dir);
        if(n == NULL){
            n = newNode(path);
        }else{
            n->right = newNode(path);
        }
        SHA256_Update(hashFile(path),ctx);
        return;
    }
    while((ent = readdir(dir) != NULL)){
        if((strcmp(ent->d_name,".") != 0) || (strcmp(ent->d_name,"..") !=0 
        && (strcmp(ent->d_name,".VMS") !=0){
            SHA256_Update(addDir(n,realpath(ent->d_name),ctx),ctx);
        }
    }
    return ctx;

    while((ent = readdir(dir)) != NULL){
        if(isFile(ent)){
            componet = fopen(ent->d_name,"rb");
            strcpy(fi.name,ent->d_name);
            fi.size =checkFileSize(componet);
            memcpy(fi.wdir,hashFile(componet));
            
            fprintf("")
        }

    }
}

Node* getFromIndex(){
    struct fileInfo** fi;
    int fileSize;
    int count;
    Node* root = NULL;
    FILE* index = fopen("./.VMS/index","rb");

    fileSize = checkFileSize(index);
    count = fileSize/sizeof(struct fileInfo);

    fi = (struct fileInfo**)malloc(sizeof(struct fileInfo*)*count);
    for(int i = 0; i < count;i++){
        fi[i] = malloc(sizeof(struct fileInfo));
    }
    for(int i = 0; i < count; i++){
        fread(fi[i],sizeof(struct fileInfo),index);
        insert(root,fi[i]);
    }
    fclose(index);
    return root;
}

void add(int argc,char* argv[]){
    struct dirent* ent;
    unsigned int fileSize;
    unsigned char zero = 0x0;

    DIR *dir = opendir("./");
    FILE *fp;
    Node* root = NULL;

    char** digest = NULL;
    char* temp;
    
    if(checkFile("./.VMS")){
        if(checkFile("./.VMS/index")){
            root = getFromIndex();
        }
        fp = open("./.VMS/index","wb");

        for(int i=1;i<argc;i++){
            SHA256_CTX ctx;
            SHA256_Init(&ctx);
            Node* n = NULL;
            if(isFile(argv[i]){
    
            }else{
                Nod
                addDir(fp,argv[i]);
            }
        }
    }
    closedir(dir);
    fclose(fp);
}



int main(int argc,char* argv[]){
    add(argc,argv);
    return 0;
}
