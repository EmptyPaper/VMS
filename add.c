#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
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

typedef struct hash_Node{
    struct hash_Node *next;
    char name[__DARWIN_MAXNAMLEN];
    int size;
    unsigned char hash[32];
    
}hashNode;

hashNode insertHashNode(hashNode root,const char* hash,char* name,int size){
    if(hash == NULL)
        return NULL;
    if(root == NULL){
        root = (hashNode*)malloc(sizeof(hashNode));
        memcpy(root->hash,hash,32);
        strcpy(root->name, name);
        root->size = size;
        return root;
    }else{
        root-> next = insertHashNode(root,hash);
    }

}
void createBlobObject(char* fileName,char* objectName){
    FILE* fp;
    gzFile* object;
    char buffer[256];
    int readByte;
    fp = fopen(fileName, "rb");
    if(fp == NULL)
        perror("fd error");
    object = gzopen(objectName,"wb");
    if(object == NULL)
        perror("zlib error");
    while((readByte = fread(buffer,1,256,fp) > 0)){
        if(gzwrite(object,buffer,256) < 0)
            perror("gzwrite error");
    }
    gzclose(object);
    fclose(fileName);
    fprintf(stderr,"object CREATED -> %s",objectName);
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

unsigned char* addFile(FILE* fp,const char* fileName,Node* root){
    Node* s;
    struct fileInfo* index;
    struct fileInfo fi;
    unsigned char *hash;
    char dir[100]; //Just apporxiamted
    char path[100];

    FILE* object;
    gzFile * comObj;

    hash = hashFile(fileName);
    sprintf(dir,"./.VMS/objects/%02x%02x",hash[0],hash[1]);
    sprintf(path,"%s/",dir);
    for(int i=2; i < 32; i++){                // Hash to char*
            sprintf(path, "%02x",hash[i]); 
    }
    if(checkfile(dir)){     //first two char of hash dir is exixted
        fprintf(stderr,"%s -> ");
        if(checkFile(path)){  //Object is Existed
                fprintf(stderr,"%s\nObject Existed\n",path+19);
                return hash;
        }else{
            /* object is not exist */
        }
    }else{
        mkdir(dir, "S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH");
    }
    createBlobObject(fileName,path);

    memset(&fi,0,sizeof(struct fileInfo));
    strcpy(fi.name,path);
    /* zlib hrer*/
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
int addTree(const char* dirName){
    DIR* dir = opendir(dirName);
    struct dirent* ent;
    while((ent = readdir(dir)) != NULL){
        
    }
}

char* add(const char* dirName){
    DIR* dir = opendir(dirName);
    size_t path_len = strlen(dirName);
    struct dirent *path;
    struct stat statbuf;
    char *buf;
    size_t len;
    hashNode hashs = NULL;
    int objNum=0;
    int size = 0;
    char* treeHash;

    if (dir){
        while ((path=readdir(dir)) != NULL){
            if (!strcmp(path->d_name, ".") || !strcmp(path->d_name, ".." 
            || !strcmp(path->d_name, ".VMS"))){
                continue;
            }
            len = path_len + strlen(p->d_name) + 2; 
            buf = malloc(len);
            if (buf){
                snprintf(buf, len, "%s/%s", dirName, path->d_name);
                if (!stat(buf, &statbuf)){
                    if (S_ISDIR(statbuf.st_mode))
                        hashs = insertHashNode(hashs,add(buf),d->name, statbuf.st_size);
                    else
                        hashs = insertHashNode(hashs, addFile(buf),d->name, statbuf.st_size);
                }
                free(buf);
            }
            size+=statbuf.st_size;
            objNum++;
        }
        closedir(dir);
    }
    else {
        fprintf(stderr,"no such object(s) in your repository\n");
    }
    /*whata actual do in add TREE*/
    hashNode* temp;
    temp = hashs;
    char* treeobj = (char*)malloc(sizeof(char)*objNum<<5 + ); //* 32
    sprintf(treeobj,"Tree %d%c\nblob%c",'\0',objNum,'\0');
    while(temp!=NULL){
        sprintf(treeobj,"\n");
        snprintf(treeobj,32,"%s",temp->hash);
        temp = temp->next;
    }
    treeHash = hashToString(hashStrings(treeobj));

    /*free somethings here*/
    free(dirName);
    return treeHash;
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

void addCmd(int argc,char* argv[]){
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
