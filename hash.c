#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#define BUFSIZE 1024
void* hashSha256(void * data);

// int main(int argc,char* argv[]){
//     char * data = "Hello";
//     unsigned char * hashString;
//     char * code;
//     hashString = hashSha256(data);
//     fprintf(stderr,"0x");
//     for(int i =0 ; i < 32; i++){
//         fprintf(stderr, "%x", hashString[i]);
//     }
//     printf("\n");
// }
void* hashSha256(void * data,FILE *fp){
    size_t size = strlen(data);
    SHA256_CTX ctx;
    char* buffer = (char*)malloc(sizeof(char)*BUFSIZE);
    unsigned char* hashout = (unsigned char*) malloc(sizeof(char)*32);
    int bytesRead = 0;
    // int count = size/32;
    SHA256_Init(&ctx);
    while((bytesRead=fread(buffer,1,BUFSIZE, fp))){
        SHA256_Update(&ctx,buffer,bytesRead);
    }
    SHA256_Update(&ctx,data,size);
    SHA256_Final(hashout, &ctx);
    return hashout;
}
