#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <zlib.h>
#include <sys/stat.h>
#include "index.h"
#include "hash.h"
#include "genKey.h"
#include "attr.h"

char* upperDir(char* path){
    char* dir = malloc(255);
    char *c = strrchr(path,'/');
    unsigned int offset = c - path;
    snprintf(dir,offset+1,"%s",path);
    dir[offset] = '\0';
    return dir;
}
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
int createTreeObject(unsigned char* hash,char* context,char* up){
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
    return flag;
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
char* dirCheck(){
    char* objectContext;
    unsigned char* hash;
    char objectName[256];
    int flag=-1;
    int dup=-1;
    contents *del;
    char* path;
    char* up;
    int first =0;
    struct container** contain = malloc(sizeof(struct container*));
    *contain = NULL;
    while(flag != 0){
        flag = getLeafDir(ROOT,contain);
        objectContext = (char*)malloc(sizeof(char)*128*PUSHCOUNT);
        while((del = pop(contain)) != NULL){
            path = del->content->name;
            if(!strcmp(del->content->name,"./commit")){
            // fprintf(stderr,"poping %s %d\n",del->content->name, del->color);
            // fprintf(stderr,"Parent %s %d\n",del->parent->content->name, del->parent->color);
            // fprintf(stderr,"SIbling %s %d\n",del->parent->left->content->name,del->parent->left->color);
            // fprintf(stderr,"P'Sibling %s %d\n",del->parent->parent->left->content->name,del->parent->parent->left->color);
            // fprintf(stderr,"new paprent %s %d\n",del->parent->parent->content->name ,del->parent->parent->color);
            }

            sprintf(objectContext+strlen(objectContext),"%s %s ",del->content->type, del->content->name);
            for(int i = 0; i<32;i++){
                sprintf(objectContext+strlen(objectContext),"%02x",del->content->hash[i]);
            }
            sprintf(objectContext+strlen(objectContext),"\n");
            red_black_delete(del);
        }
        objectContext[strlen(objectContext)-1]='\0';
        up = upperDir(path);
        hash = hashchars(objectContext);

        dup=createTreeObject(hash,objectContext,up);
        free(objectContext);
    }
   
    objectContext = malloc(sizeof(char)*65);
    for(int i = 0; i<32;i++){
        sprintf(objectContext+strlen(objectContext),"%02x",hash[i]);
    }
    if(dup == 1){
        fprintf(stderr,"already up-to-date hash -> %s\n",objectContext);
        exit(1);
    }
    fprintf(stderr,"root tree's hash : %s\n", objectContext);
    return objectContext;
}
void Log(char* head,char* email, char* nick,char* parentCommit,char* commit,char* commitMsg){
    FILE* HEAD;
    FILE* log;
    FILE* config;

    char logPath[50]={0,};
    char line[256];

    sprintf(logPath,"./.VMS/log/heads/%s",head);
    /* work of Logging */
    if(access(logPath,F_OK) != -1){
        log = fopen(logPath,"r+");
        fseek(log,0,SEEK_END);
        fprintf(log,"\n");
    }else{
        log = fopen(logPath,"w");
    }
    fprintf(log,"%s %s \t%s \t<%s> commit: %s",parentCommit, commit, nick+5,email+6,commitMsg);
    fclose(log);
     /* work of HEAD Update */
}
unsigned char* createCommitObject(char* email,char* nick,char* parentCommit,char* rootTree,char* commitMsg){
    gzFile object;
    unsigned char* hash;
    char commit[512]={0,};
    char dir[100];
    char path[256];
    unsigned char* sign;
    char* encoded;

    rootTree[OBJECT_NAME_LENGHT]='\0';
    sprintf(commit,"parent %s\n",parentCommit);
    sprintf(commit+strlen(commit),"tree %s",rootTree);
    sprintf(commit+strlen(commit),"\ncommiter %s <%s>\n",nick+5,email+6);
    hash = hashchars(commit);
    sprintf(commit+strlen(commit),"msg %s",commitMsg);
    // fprintf(stderr,"%s\n",commit);
    sign = signification(commit);

    Base64Encode(sign,128,&encoded);

    sprintf(commit+strlen(commit),"key\n");
    sprintf(commit+strlen(commit),"%s",encoded);
    

    fprintf(stderr,"%s\n",commit);


    sprintf(dir,"./.VMS/objects/%02x%02x",hash[0],hash[1]);
    sprintf(path,"%s/",dir);

    for(int i=2; i < 32; i++){                // Hash to char*
            sprintf(path+strlen(path), "%02x",hash[i]); 
    }
    if(access(dir, F_OK) != -1 ){     //first two char of hash dir is exixted
        // fprintf(stderr,"%s -> ",path);
        if(access(path, F_OK) != -1){  //Object is Existed
                fprintf(stderr,"%s already up-to-date\n",path);
                exit(1);
        }
    }else{
        mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    object = gzopen(path,"wb");
    if(object==NULL)
        perror("gzwrite error");
    if(gzwrite(object,commit,strlen(commit)) < 0)
        perror("gzwrite error");
    gzclose(object);
    fprintf(stderr,"%s <- Commit object CREATED\n",path);
    return hash;
}
void commit(char* commitMsg){
    char* rootTree;
    FILE* HEAD;
    FILE* HEADCommit;
    FILE* config;
    FILE* log;
    unsigned char* hash;
    char* commit;
    char email[EMAIL_LENGHT];
    char nick[NICK_LENGHT];
    char headPath[256]={0,};
    char logPath[256]={0,};
    char *head = malloc(HEAD_NAME_LENGHT+6);
    char parentCommit[COMMIT_HASH_LENGHT+1];
    // char dup[256]={0,};
    char line[1024];
    /*load fils from index */
    loadIndex();
    if(ROOT==NILL)
        perror("./.VMS/index corrupted");
    /* recursively create Tree Objects */
    rootTree = dirCheck();
    // sprintf(dup,"./.VMS/objects/%c%c%c%c/",rootTree[0],rootTree[1],rootTree[2],rootTree[3]);
    // sprintf(dup+strlen(dup),"%s",rootTree+4);
    // if(access(dup,F_OK)!=-1){
    //     fprintf(stderr,"already up-to-date\n");
    //     exit(1);
    // }
    /*-----------------------------------------------------------------*/
    /* get HEAD */
    HEAD = fopen("./.VMS/HEAD","r");
    if(HEAD==NULL){
        fprintf(stderr,"HEAD open error\n");
        exit(1);
    }
    fseek(HEAD,6,SEEK_SET);
    fgets(head,HEAD_NAME_LENGHT,HEAD);

    /* get REAL HEAD */
    snprintf(logPath,200,"./.VMS/log/heads/%s",head);
    log = fopen(logPath,"r+");
    /* head is not exist */
    if(log == NULL){
        for(int i=0;i<COMMIT_HASH_LENGHT;i++)
            parentCommit[i]='0';
    }
    else{
        while(!feof(log))
            fgets(line,1024,log);
        strncpy(parentCommit,line+65,COMMIT_HASH_LENGHT);
    }

    parentCommit[COMMIT_HASH_LENGHT]='\0';
    /* get config */
    config = fopen("./.VMS/Info","r");
    fgets(email,EMAIL_LENGHT-1,config);
    email[strlen(email)-1]='\0';
    fgets(nick,NICK_LENGHT-1,config);
    nick[strlen(nick)-1]='\0';
    /* create commit object  & update real HEAD*/
    hash = createCommitObject(email,nick,parentCommit,rootTree,commitMsg);
    commit = hashToString(hash);
    // fseek(HEADCommit, 0, SEEK_SET);
    
    // HEADCommit = fopen(headPath,"r+");
    // if(HEADCommit == NULL){
    
    snprintf(headPath,200,"./.VMS/refers/heads/%s",head);
    HEADCommit = fopen(headPath,"w");
    // }
    // else{
    //     fgets(parentCommit,COMMIT_HASH_LENGHT,HEADCommit);
    // }
    fprintf(HEADCommit,"%s",commit);
    fclose(log);
    fclose(HEAD);
    fclose(HEADCommit);
    fclose(config);
    Log(head,email,nick,parentCommit,commit,commitMsg);
    free(head);
}
void commitCmd(char* commitMsg){
    /* if index file is NOT exist */
    NILL = malloc(sizeof(contents));
    NILL->color = BLACK;
    NILL->right = NILL->left =NULL;
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
    commit(commitMsg);
}
int main(int argc,char* argv[]){
    if(argc != 2){
        fprintf(stderr,"usage : commit \"commit message\"\n");
        return 0;
    }
    /* add consttrain*/
    commitCmd(argv[1]);
}
