#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#define BUFSIZE 1024
void* hashFile(FILE *fp);
void *hashStrings(void **string);
void *hashchars(void* string);


char* hashToString(unsigned char* hash){
    char string[65];
    for(int j =0 ; j < 32; j++){
        fprintf(string, "%02x", hash[j]);
    }
    return string;
}


int main(int argc,char* argv[]){
//     char * data = "Hello";
    unsigned char ** hashString = malloc(sizeof(char*)*3);
//     char * code;
    unsigned char output[97];
    unsigned char * z;

    FILE** fp;
    fp = (FILE**)malloc(sizeof(FILE*)*3);
    fp[0] = fopen("./init.c","rb");
    fp[1] = fopen("./ds.h","rb");
    fp[2] = fopen("./HEAD","rb");

    // fp[1] = fopen("./init.c", "rb");
    if(fp == NULL)
        perror("file error");
    for(int i =0; i< 3; i++){
        memcpy(output+i*32,hashFile(fp[i]),32);
        for(int j =0 ; j < 32; j++){
            fprintf(stderr, "%02x", output[i*32+j]);
        }
        fprintf(stderr,"\n");
    }
    output[96] = '\0';
    for(int i = 0; i < 96; i++){
        fprintf(stderr,"%d",output[i]);
    }
    fprintf(stderr,"\n");
    z = hashchars(output);
    // output = hashStrings(hashString);

    for(int j =0 ; j < 32; j++){
            fprintf(stderr, "%02x", z[j]);
    }
    fprintf(stderr,"\n");
    // hashString [0]= hashSha256(fp);
    // fprintf(stderr,"./init.c");
    // for(int i =0 ; i < 32; i++){
    //     fprintf(stderr, "%x", hashString[i]);
    // }
     printf("\n");
}

void* hashFile(char *path){
    // size_t size = strlen(data);
    FILE* fp = fopen(path,"rb");
    SHA256_CTX ctx;
    char* buffer = (char*)malloc(sizeof(char)*BUFSIZE);
    unsigned char* hashout = (unsigned char*) malloc(sizeof(char)*32);
    int bytesRead = 0;
    struct stat statInfo;
    char blobHeader[30];

    if(stat(path,&statInfo))
        perror("error");
    blobHeader = (char*)malloc(sizeof(char)*(5+statInfo.st_size/10+1));
    sprintf(blobHeader,"blob %d%c\n",statInfo.st_size,'\0');
    SHA256_Init(&ctx);
    SHA256_Update(&ctx,blobHeader,szieof(blobHeader));

    while((bytesRead=fread(buffer,1,BUFSIZE, fp))){
            SHA256_Update(&ctx,buffer,bytesRead);
            memset(buffer,0,BUFSIZE);
    }
    if(bytesRead < 0)
        perror("file read error");
    SHA256_Final(hashout, &ctx);

    fclose(fp);
    return hashout;
}

void *hashStrings(void **string){
    SHA256_CTX ctx;
    unsigned char* hashout =(unsigned char*)malloc(sizeof(char)*SHA256_DIGEST_LENGTH);
    SHA256_Init(&ctx);
    for(int i = 0 ; i < 3;i++)
        SHA256_Update(&ctx,string[i],strlen(string[i]));
    SHA256_Final(hashout, &ctx);

    return hashout;
}
void *hashchars(void* string){
    SHA256_CTX ctx;
    unsigned char* hashout =(unsigned char*)malloc(sizeof(char)*SHA256_DIGEST_LENGTH);
    SHA256_Init(&ctx);
    SHA256_Update(&ctx,string,strlen(string));
    SHA256_Final(hashout, &ctx);
    return hashout;
}