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

void add(char* dirName){
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
            || !strcmp(path->d_name, ".VMS") || !strcmp(path->d_name,".git")){
                continue;
            }
            len = path_len + strlen(path->d_name) + 2; 
            buf = malloc(len);
            if (buf){
                snprintf(buf, len, "%s/%s", dirName, path->d_name);
                if (!stat(buf, &statbuf)){
                    if (S_ISDIR(statbuf.st_mode))
                        add(buf);
                    else{
                        content = malloc(sizeof(indexContent));
                        content->conflict = 0;
                        strcpy(content->name,buf);
                        memcpy(content->hash,addFile(buf),32);
                        strcpy(content->type, "blob");
                        content->size = statbuf.st_size;
                        red_black_insert(content);
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
            red_black_insert(content);
        }
    }
    // treeHash = addTree(hashs, objNum);
    /*free somethings here*/
    // free(dirName);
}
void addCmd(int argc,char* argv[]){
    struct dirent* ent;
    unsigned int fileSize;
    unsigned char zero = 0x0;
    NILL = malloc(sizeof(contents));
    NILL->color=BLACK;
    ROOT = NILL;
    if(access("./.VMS",F_OK) != -1){
        if(access("./.VMS/index",F_OK) != -1){
            loadIndex();
        }
        for(int i=1;i<argc;i++){
            add(argv[i]);
        }
        gzFile index;
        index = gzopen("./.VMS/index","wb");
        saveIndex(ROOT,&index);
        gzclose(index);
    }else{
        perror("NO initiation");
    }

}

int main(int argc,char* argv[]){
    addCmd(argc,argv);
    return 0;
}
