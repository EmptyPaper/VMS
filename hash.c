#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <sys/stat.h>
#include <assert.h>
#include <openssl/buffer.h>
#include <stdint.h>

#include "hash.h"

char* hashToString(unsigned char* hash){
    char* string = malloc(sizeof(char)*64);
    for(int j =0 ; j < 32; j++){
        sprintf(string+strlen(string), "%02x", hash[j]);
    }
    return string;
}

size_t calcDecodeLength(const char* b64input) { //Calculates the length of a decoded string
	size_t len = strlen(b64input),
		padding = 0;

	if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
		padding = 2;
	else if (b64input[len-1] == '=') //last char is =
		padding = 1;

	return (len*3)/4 - padding;
}

int Base64Decode(char* b64message, unsigned char** buffer, size_t* length) { //Decodes a base64 encoded string
	BIO *bio, *b64;

	int decodeLen = calcDecodeLength(b64message);
	*buffer = (unsigned char*)malloc(decodeLen + 1);
	(*buffer)[decodeLen] = '\0';

	bio = BIO_new_mem_buf(b64message, -1);
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
	*length = BIO_read(bio, *buffer, strlen(b64message));
	assert(*length == decodeLen); //length should equal decodeLen, else something went horribly wrong
	BIO_free_all(bio);

	return (0); //success
}
int Base64Encode(const unsigned char* buffer, size_t length, char** b64text) { //Encodes a binary safe base 64 string
	BIO *bio, *b64;
	BUF_MEM *bufferPtr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
	BIO_write(bio, buffer, length);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bufferPtr);
	BIO_set_close(bio, BIO_NOCLOSE);
	BIO_free_all(bio);

	*b64text=(*bufferPtr).data;

	return (0); //success
}



// int main(int argc,char* argv[]){
// //     char * data = "Hello";
//     unsigned char ** hashString = malloc(sizeof(char*)*3);
// //     char * code;
//     unsigned char output[97];
//     unsigned char * z;

//     FILE** fp;
//     fp = (FILE**)malloc(sizeof(FILE*)*3);
//     fp[0] = fopen("./init.c","rb");
//     fp[1] = fopen("./ds.h","rb");
//     fp[2] = fopen("./HEAD","rb");

//     // fp[1] = fopen("./init.c", "rb");
//     if(fp == NULL)
//         perror("file error");
//     for(int i =0; i< 3; i++){
//         memcpy(output+i*32,hashFile(fp[i]),32);
//         for(int j =0 ; j < 32; j++){
//             fprintf(stderr, "%02x", output[i*32+j]);
//         }
//         fprintf(stderr,"\n");
//     }
//     output[96] = '\0';
//     for(int i = 0; i < 96; i++){
//         fprintf(stderr,"%d",output[i]);
//     }
//     fprintf(stderr,"\n");
//     z = hashchars(output);
//     // output = hashStrings(hashString);

//     for(int j =0 ; j < 32; j++){
//             fprintf(stderr, "%02x", z[j]);
//     }
//     fprintf(stderr,"\n");
//     // hashString [0]= hashSha256(fp);
//     // fprintf(stderr,"./init.c");
//     // for(int i =0 ; i < 32; i++){
//     //     fprintf(stderr, "%x", hashString[i]);
//     // }
//      printf("\n");
// }

void* hashFile(char *path){
    // size_t size = strlen(data);
    FILE* fp = fopen(path,"rb");
    SHA256_CTX ctx;
    char* buffer = (char*)malloc(sizeof(char)*BUFSIZE);
    unsigned char* hashout = (unsigned char*) malloc(sizeof(char)*32);
    int bytesRead = 0;
    struct stat statInfo;
    char *blobHeader;

    if(stat(path,&statInfo))
        perror("error");
    blobHeader = (char*)malloc(sizeof(char)*(7+statInfo.st_size/10+1));
    sprintf(blobHeader,"blob %lld%c\n",statInfo.st_size,'\0');
    SHA256_Init(&ctx);
    SHA256_Update(&ctx,blobHeader,sizeof(blobHeader));

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