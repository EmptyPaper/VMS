#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <zlib.h>
#include <sys/stat.h>
#include "index.h"


unsigned int checkRepository();

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
void createTreeObject(char* contents,char* objectName){
    gzFile object;
    int readByte;
    object = gzopen(objectName,"wb");
    if(object==NULL)
        perror("gzwrite error");
    if(gzwrite(object,contents,sizeof(contents)) < 0)
        perror("gzwrite error");
    gzclose(object);
    fprintf(stderr,"Tree object CREATED -> %s",objectName);
}
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
void inOrderCheck(contents* hashs){
    indexContent* content;
    static struct container* contain;
    static contents* dir = NULL;
    static unsigned int before = 0;
    static char* before_dir = NULL; 
    unsigned int dirOffset=0;
    static int stackCOunt = 0;
    char* root;
    char* s;
    if(hashs!=NULL){
        inOrderCheck(hashs->left);
        s = strrchr(hashs->content->name,'/');
        dirOffset = s - hashs->content->name;
        // strncpy(before_dir,hashs->content->name,dirOffset);
        // fprintf(stderr,"dirOffset :  %s\n",before_dir);
        // root = isSame(before_dir,hashs->content->name);
        // root = upperDir(hashs->content)
        // fprintf(stderr,"root : %s", root);
        
        if(!strncmp(before_dir,hashs->content->name,before) && before == dirOffset){
            fprintf(stderr,"Pushing %s \t: %d\n",hashs->content->name,stackCOunt);
            indexInsert(&dir,hashs->content);
        }
        else{
            // createTreeObject(dir,);
            if(before>dirOffset){
                dir = pop(&contain);
                stackCOunt--;
            }
            else{
            push(&contain,dir);
            stackCOunt++;
            dir = NULL;
            indexInsert(&dir,hashs->content);
            }
            fprintf(stderr,"%s  <- chdir\t: %d\n",hashs->content->name,stackCOunt);

        }
        // else{
        //     fprintf(stderr,"error]");
        // }
        // if(!memcpy(content->name,hashs->content->name,strlen(content->name)))
        //     if(hashs->content->name)
        // if((s = strchr(hashs->content->name, content->name))!=NULL)
        before = dirOffset;
        before_dir = hashs->content->name;
        inOrderCheck(hashs->right);
    }
    
    // return dirOffset;
}
char* upperDir(char* path){
    char* dir = malloc(255);
    char *c = strrchr(path,'/');
    unsigned int offset = c - path;
    strncpy(dir,path,offset);
    return dir;
}


// if(hashs != NULL){
//                 dirCommit(dir,hashs);
//                 hashs = NULL;
// //             }
// char* coDir(char* c1,char* c2){
//     const char *tok = "/";
//     char* p1 = strtok(c1->name,tok);
//     char* p2 = strtok(c1->name,tok);
//     char* r=c1;
//     if(p1!=NULL && (strcmp(p1,p2) == 0) ){
//         r = coDir(p1,p2);
//     }
//     strcat(c1,)
//     return r; 
// }
void commit(contents** hashs){
    //char* temp = (*hashs)->content->name;
    loadIndex(hashs);
    fprintf(stderr,"%d %d\n",(*hashs)->left,(*hashs)->right);
    if(*hashs==NULL)
        perror("./.VMS/index corrupted");
    inOrderCheck(*hashs);
}
void commitCmd(){
    /* if index file is NOT exist */
    contents** hashs = (contents**)malloc(sizeof(contents*));
    *hashs = NULL;
    
    if(access("./.VMS", F_OK) == -1){
        fprintf(stderr,".VMS is NOT EXIST\n");
        fprintf(stderr,"please init VMS first\n");
        return;
    } else if(access("./.VMS/index", F_OK) == -1){ 
        fprintf(stderr,"INDEX is NOT EXIST\n");
        fprintf(stderr,"please INDEXING VMS first");
        return;
    } 
    commit(hashs);
}
int main(int argc,char* argv[]){
    commitCmd();
}
