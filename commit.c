#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <zlib.h>
#include <sys/stat.h>
#include "index.h"
#include "hash.h"


unsigned int checkRepository();

char* upperDir(char* path){
    char* dir = malloc(255);
    char *c = strrchr(path,'/');
    unsigned int offset = c - path;
    snprintf(dir,offset+1,"%s",path);
    dir[offset] = '\0';
    return dir;
}
// void vector(){

// }
// void hashChain(){
//     FILE *HEAD;
//     char buffer[256];
//     char code_name[64];
//     if(checkRepository()){
//         HEAD = fopen("./.VMS", "r");
//         if(HEAD == NULL){
//             perror("VMS FILE IS CONTAMINATED");
//         }
//         fgets(buffer,256, HEAD);
//         if(strcmp(buffer,"ref : master") != 0){
//             fprintf(stderr,"first commit");
//         }
//         else{
//             strcpy(code_name,buffer+13);
            
//         }
//     }
//     else{
//         perror("VMS is not initiated");
//     }

// }

// unsigned char* addTree(hashNode* hashs, int objNum){
//     hashNode* temp;
//     temp = hashs;
//     unsigned char* treeHash;
//     char* treeobj = (char*)malloc((sizeof(char))*objNum<<8); //* 32
//     char dir[100]; //Just apporxiamted
//     char path[100];

//     sprintf(treeobj,"Tree %d%c\nblob ",objNum,'\0');

//     while(temp!=NULL){
//         sprintf(treeobj,"\n");
//         snprintf(treeobj,32,"blob %s\0 %s",temp->name,temp->hash);
//         temp = temp->next;
//     }
//     treeHash = hashchars(treeobj);

//     sprintf(dir,"./.VMS/objects/%02x%02x",treeHash[0],treeHash[1]);
//     sprintf(path,"%s/",dir);
//     for(int i=2; i < 32; i++){                // Hash to char*
//             sprintf(path, "%02x",treeHash[i]); 
//     }
//     if(checkfile(dir)){     //first two char of hash dir is exixted
//         fprintf(stderr,"%s -> ");
//         if(checkFile(path)){  //Object is Existed
//                 fprintf(stderr,"%s\nObject Existed\n",path+19);
//                 return treeHash;
//         }
//     }else{
//         mkdir(dir, "S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH");
//     }
//     createTreeObject(treeobj,dir);
//     updateIndex(treeHash,hashs);
//     return hashToString(treeHash);
// // }
// void dirCommit(char* dirName,contents* content){
//     contents* itor = content;
//     struct stat statbuf;
//     while(itor!=NULL){
//         if(!stat(,statbuf)){
            
//         }
//         itor = itor->right;
//     }
// }

char* isSame(char* before, char* current){
    unsigned int before_offset;
    unsigned int current_offset;
    char* root = (char*)malloc(sizeof(char)*255);
    char *c = strrchr(current,'/');
    current_offset  = c - current;
    if(before == NULL){
        strncpy(root,current,current_offset+1);
        return root;
    }
    // fprintf(stderr,"sdfsdf%d",current_offset);

    char *b = strrchr(before,'/');
    before_offset = b - before;

    
    unsigned int len = before_offset <= current_offset ? before_offset:current_offset;
    len++;
    for(int i = 0;i <len; i++){
        if(before[i] != current[i]){
            root = malloc(255);
            strncpy(root,before,i);
            break;
        }
    }
    return root;
}
void createTreeObject(unsigned char* hash,char* context,char* up){
    gzFile object;
    int readByte;
    char dir[100];
    char path[256];
    indexContent* content;
    int flag=0;

    sprintf(dir,"./.VMS/objects/%02x%02x",hash[0],hash[1]);
    sprintf(path,"%s/",dir);

     for(int i=2; i < 32; i++){                // Hash to char*
            sprintf(path+strlen(path), "%02x",hash[i]); 
    }
    if(access(dir, F_OK) != -1 ){     //first two char of hash dir is exixted
        // fprintf(stderr,"%s -> ",path);
        if(access(path, F_OK) != -1){  //Object is Existed
                fprintf(stderr,"%s -> Object Existed\n",path);
                flag=1;
        }
    }else{
        mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    if(flag == 0){
        object = gzopen(path,"wb");
        if(object==NULL)
            perror("gzwrite error");
        if(gzwrite(object,context,strlen(context)) < 0)
            perror("gzwrite error");
        gzclose(object);
        fprintf(stderr,"%s <- Tree object CREATED\n",path);
    }
    /* AND INSERT INTO TREE */
    content = malloc(sizeof(indexContent));
    memcpy(content->hash, hash,32);
    strcpy(content->name, up);
    strcpy(content->type,"tree");
    content->size = -1;
    content->conflict = 0;
    // fprintf(stderr,"new tree object : %s\n",content->name);
    red_black_insert(content);
}
int getLeafDir(contents *head,struct container** contain){
    static int beforeDepth = 0;
    contents* del;
    static int dbgCount;
    int terminator=0;
    if(head!=NILL){
        terminator= getLeafDir(head->left,contain);
        if(terminator != 0)
            return terminator;
        dbgCount++;
        int depth = countDepth(head->content->name);
        if(beforeDepth!= 0 && beforeDepth > depth){
            // fprintf(stderr,"break!! dbgcont : %d depth : %d\n",dbgCount,depth);
            beforeDepth = 0;
            // while((del = pop(contain)) != NULL){
            //     fprintf(stderr,"poping %s \n",del->content->name);
            //     red_black_delete(del);
            //     fprintf(stderr,"complete delete\n");
            // }
            terminator++;
            return terminator; // no return end all sequence;
        }else{
            // fprintf(stderr,"pushgin %s %d\n",head->content->name,head->color);
            push(contain,head);
            // red_black_delete(head);
            beforeDepth = depth;
        }
        terminator=getLeafDir(head->right,contain);
        if(terminator!=0){
            return terminator;
        }
    }
    return 0;
}
void createRootTreeObject(unsigned char* hash,char* objectContext){

}
char* dirCheck(){
    char* objectContext;
    unsigned char* hash;
    char objectName[256];
    int flag=-1;
    contents *del;
    char* path;
    char* up;
    struct container** contain = malloc(sizeof(struct container*));
    *contain = NULL;
    while(flag != 0){
        flag = getLeafDir(ROOT,contain);
        objectContext = (char*)malloc(sizeof(char)*128*PUSHCOUNT);
        sprintf(objectContext,"Tree");
        while((del = pop(contain)) != NULL){
            path = del->content->name;
            if(!strcmp(del->content->name,"./commit")){
            // fprintf(stderr,"poping %s %d\n",del->content->name, del->color);
            // fprintf(stderr,"Parent %s %d\n",del->parent->content->name, del->parent->color);
            // fprintf(stderr,"SIbling %s %d\n",del->parent->left->content->name,del->parent->left->color);
            // fprintf(stderr,"P'Sibling %s %d\n",del->parent->parent->left->content->name,del->parent->parent->left->color);
            // fprintf(stderr,"new paprent %s %d\n",del->parent->parent->content->name ,del->parent->parent->color);
            }

            sprintf(objectContext+strlen(objectContext),"\n");
            sprintf(objectContext+strlen(objectContext),"%s %s ",del->content->type, del->content->name);
            for(int i = 0; i<32;i++){
                sprintf(objectContext+strlen(objectContext),"%02x",del->content->hash[i]);
            }
            red_black_delete(del);
             fprintf(stderr,"complete delete\n");
        }
        up = upperDir(path);
        hash = hashchars(objectContext);
        createTreeObject(hash,objectContext,up);
        free(objectContext);
    }
    objectContext = malloc(sizeof(char)*65);
    for(int i = 0; i<32;i++){
        sprintf(objectContext+strlen(objectContext),"%02x",hash[i]);
    }
    fprintf(stderr,"root tree's hash : %s\n", objectContext);
    return objectContext;
}
void commit(){
    char* rootTree;
    loadIndex();
    if(ROOT==NILL)
        perror("./.VMS/index corrupted");
    rootTree = dirCheck();
    
}
void commitCmd(){
    /* if index file is NOT exist */
    NILL = malloc(sizeof(contents));
    NILL->color = BLACK;
    ROOT = NILL;
    PUSHCOUNT = 0;
    
    // contents** hashs = (contents**)malloc(sizeof(contents*));
    // *hashs = NILL;
    
    if(access("./.VMS", F_OK) == -1){
        fprintf(stderr,".VMS is NOT EXIST\n");
        fprintf(stderr,"please init VMS first\n");
        return;
    } else if(access("./.VMS/index", F_OK) == -1){ 
        fprintf(stderr,"INDEX is NOT EXIST\n");
        fprintf(stderr,"please INDEXING VMS first");
        return;
    } 
    commit();
}
int main(int argc,char* argv[]){
    commitCmd();
}
