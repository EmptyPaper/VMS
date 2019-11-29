#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>

#include <openssl/sha.h>

#include "ds.h"

/* void create_dir(){
//     #ifdef __linux__
//         mkdir(".vms",777);
//     #else
//         _mkdir(".vms");
//     #endif
 } */

typedef struct hash_Node{
    struct hash_Node *next;
    char name[__DARWIN_MAXNAMLEN];
    int size;
    unsigned char hash[32];
    
}hashNode;

struct indexForm form;

void insertIndex(){

}
void updateIndex(unsigned char* Treehash,hashNode* objects){
    struct indexContent* content = form.body;
    struct indexContent* newContent = 
    (struct indexContent*)malloc(sizeof(struct indexContent)* );
    int offset;
    hashNode* temp;
    temp = objects;
    while(temp!=NULL){
        offset = findIndex(content, 0, form.head.contentNum-1, temp->name);
        if(offset == -1)
        memcpy(content[offset].wdir,Treehash,32);
        memcpy(content[offset].stage,Treehash,32);
        temp = temp->next;
    }
}

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
        }
    }else{
        mkdir(dir, "S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH");
    }
    createBlobObject(fileName, path);
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
            // statbuf.st_mtimespec;
            objNum++;
        }
        closedir(dir);
    }
    else {
        fprintf(stderr,"no such object(s) in your repository\n");
    }
    treeHash = addTree(hashs, objNum);
    /*free somethings here*/
    free(dirName);

    return treeHash;
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
void loadIndex(){
    FILE* index = fopen("./.VMS/index","rb");
    int fileSize;
    struct indexHead head;
    struct indexContent* content;
    fileSize = checkFileSize(index);
    fread(&head, sizeof(struct indexHead),1,index);
    if(head.indexSize != fileSize){
        perror(".VMS/index failed : recover");
    }
    content =(struct indexContent*)malloc(sizeof(struct indexContent)* head.contentNum);
    for(int i=0;i<head.contentNum;i++){
        fread(content+i, sizeof(struct indexContent), 1, index);
    }
    form.head = head;
    form.body = content;
    fclose(index);
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
