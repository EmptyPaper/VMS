#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>

#include <openssl/sha.h>


#include "index.h"
#include "hash.h"
/* void create_dir(){
//     #ifdef __linux__
//         mkdir(".vms",777);
//     #else
//         _mkdir(".vms");
//     #endif
 } */

// typedef struct hash_Node{
//     struct hash_Node *right,left;
//     char name[__DARWIN_MAXNAMLEN];
//     int size;
//     unsigned char hash[32];
    
// }hashNode;

// struct indexForm form;

// void updateIndex(unsigned char* Treehash,hashNode* objects){
//     struct indexContent* content = form.body;
//     struct indexContent* newContent = 
//     (struct indexContent*)malloc(sizeof(struct indexContent)* );
//     int offset;
//     hashNode* temp;
//     temp = objects;
//     while(temp!=NULL){
//         offset = findIndex(content, 0, form.head.contentNum-1, temp->name);
//         if(offset == -1)
//         memcpy(content[offset].wdir,Treehash,32);
//         memcpy(content[offset].stage,Treehash,32);
//         temp = temp->next;
//     }
// }

// hashNode* insertHashNode(hashNode* root,const char* hash,char* name,int size){
//     if(hash == NULL)
//         return NULL;
//     if(root == NULL){
//         root = (hashNode*)malloc(sizeof(hashNode));
//         memcpy(root->hash,hash,32);
//         strcpy(root->name, name);
//         root->size = size;
//         root->right = root->left = NULL;
//         return root;
//     }
//     else if(strcmp(root->name, name) < 0)
//         root->left = insertHashNode(root->left, hash, name, size);
//     else if(strcmp(root->name, name) > 0)
//         root->right = insertHashNode(root->right, hash, name, size);

// }

void createBlobObject(char* fileName,char* objectName){
    FILE* fp;
    gzFile object;
    char buffer[256];
    int readByte;
    fp = fopen(fileName, "rb");
    if(fp == NULL)
        perror("fd error");
    object = gzopen(objectName,"wb");
    if(object == NULL)
        perror("zlib error");
    while(!feof(fp)){
        memset(buffer,0,256);
        readByte = fread(buffer,1,256,fp);
        if(gzwrite(object,buffer,readByte) < 0)
            perror("gzwrite error");
    }
    // while((readByte = fread(buffer,1,256,fp)) > 0){
    //     if(gzwrite(object,buffer,readByte) == 0)
    //         perror("gzwrite error");
    //     fprintf(stderr,"%s",buffer);
    // }
    gzclose(object);
    fclose(fp);
    fprintf(stderr,"\n-> %s for\n",fileName);
    fprintf(stderr,"object CREATED -> %s\n",objectName);
}
unsigned char* addFile(char* fileName){
    unsigned char *hash;
    char dir[100]; //Just apporxiamted
    char path[256];

    FILE* object;
    gzFile comObj;

    hash = hashFile(fileName);
    sprintf(dir,"./.VMS/objects/%02x%02x",hash[0],hash[1]);
    sprintf(path,"%s/",dir);
    for(int i=2; i < 32; i++){                // Hash to char*
            sprintf(path+strlen(path), "%02x",hash[i]); 
    }
    if(access(dir, F_OK) != -1 ){     //first two char of hash dir is exixted
        // fprintf(stderr,"%s -> ",path);
        if(access(path, F_OK) != -1){  //Object is Existed
                // fprintf(stderr,"%s\nObject Existed\n",path+19);
                return hash;
        }
    }else{
        mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    fprintf(stderr,"object name : %s", path);
    createBlobObject(fileName, path);

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

void add(char* dirName, contents** hashs){
    DIR* dir = opendir(dirName);
    size_t path_len = strlen(dirName);
    indexContent* content;
    struct dirent *path;
    struct stat statbuf;
    char *buf;
    size_t len;
    int objNum=0;
    int size = 0;
    char* treeHash;

    if (dir){
        while ((path=readdir(dir)) != NULL){
            if (!strcmp(path->d_name, ".") || !strcmp(path->d_name, "..") 
            || !strcmp(path->d_name, ".VMS")){
                continue;
            }
            len = path_len + strlen(path->d_name) + 2; 
            buf = malloc(len);
            if (buf){
                snprintf(buf, len, "%s/%s", dirName, path->d_name);
                if (!stat(buf, &statbuf)){
                    if (S_ISDIR(statbuf.st_mode))
                        add(buf,hashs);
                    else{
                        content = malloc(sizeof(indexContent));
                        content->conflict = 0;
                        strcpy(content->name,buf);
                        memcpy(content->hash,addFile(buf),32);
                        content->size = statbuf.st_size;
                        indexInsert(hashs, content);
                    }
                }
                free(buf);
            }
            // size+=statbuf.st_size;
            // statbuf.st_mtimespec;
            objNum++;
        }
        closedir(dir);
    }
    else {
        if (!stat(dirName, &statbuf)){
            content = malloc(sizeof(indexContent));
            content->conflict = 0;
            strcpy(content->name,dirName);
            memcpy(content->hash,addFile(dirName),64);
            content->size = statbuf.st_size;
            indexInsert(hashs, content);
        }
    }
    // treeHash = addTree(hashs, objNum);
    /*free somethings here*/
    // free(dirName);
}
// Node* getFromIndex(){
//     struct fileInfo** fi;
//     int fileSize;
//     int count;
//     Node* root = NULL;
//     FILE* index = fopen("./.VMS/index","rb");

//     fileSize = checkFileSize(index);
//     count = fileSize/sizeof(struct fileInfo);

//     fi = (struct fileInfo**)malloc(sizeof(struct fileInfo*)*count);
//     for(int i = 0; i < count;i++){
//         fi[i] = malloc(sizeof(struct fileInfo));
//     }
//     for(int i = 0; i < count; i++){
//         fread(fi[i],sizeof(struct fileInfo),index);
//         insert(root,fi[i]);
//     }
//     fclose(index);
//     return root;
// }

void addCmd(int argc,char* argv[]){
    struct dirent* ent;
    unsigned int fileSize;
    unsigned char zero = 0x0;
    contents** hashs = (contents**)malloc(sizeof(contents*));
    *hashs = NULL;

    if(access("./.VMS",F_OK) != -1){
        if(access("./.VMS/index",F_OK) != -1){
            loadIndex(hashs);
        }
        for(int i=1;i<argc;i++){
            add(argv[i],hashs);
        }
        gzFile index;
        index = gzopen("./.VMS/index","wb");
        saveIndex(*hashs,&index);
        gzclose(index);
    }else{
        perror("NO initiation");
    }

}

int main(int argc,char* argv[]){
    addCmd(argc,argv);
    return 0;
}
