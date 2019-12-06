#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genKey.h"
#include "hash.h"

RSA * createRSA(unsigned char * key,int public){
    RSA *rsa= NULL;
    BIO *keybio;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio==NULL)
    {
        printf( "Failed to create key BIO");
        return 0;
    }
    if(public)
    {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
    }
    else
    {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
    }
 
    return rsa;
}
void genPrivateKey(){

    const int kBits = 1024;
    const int kExp = 3;
    int keylen;
    char *pem_key;

    RSA *rsa = RSA_generate_key(kBits, kExp, 0, 0);
    FILE *fp = fopen("user.key","wb");
    if(fp == NULL)
        perror("file open error");
    
    /* To get the C-string PEM form: */
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPrivateKey(bio, rsa, NULL, NULL, 0, NULL, NULL);
    keylen = BIO_pending(bio);
    pem_key = calloc(keylen+1, 1); /* Null-terminate */
    BIO_read(bio, pem_key, keylen);
    printf("%s", pem_key);

    // fwrite(pem_key,1,strlen(pem_key),fp);
    PEM_write_RSAPrivateKey(fp,rsa,NULL,NULL,0,NULL,NULL);
    // fprintf(fp,"%s",pem_key);
    BIO_free_all(bio);
    RSA_free(rsa);
    fclose(fp);
    free(pem_key);
}
char* genPublicKey(){
    int PKeyLen;
    char *public_key;
    FILE* fp;
    RSA *rsa= NULL;
    BIO *keybio;
    BIO *bio = BIO_new(BIO_s_mem());
    fp = fopen("user.key","rb");
    if(fp==NULL)
        perror("user.Key is NOT EXIST");

    // fread(privateKey,1,887,fp);
    // keybio = BIO_new_mem_buf(privateKey, -1);
    // if (keybio==NULL)
    //     perror( "Failed to create key BIO");
    // if(public)
    // {
    //     rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
    // }
    // rsa = PEM_read_bio_RSA_PUBKEY(keybio,&rsa,NULL,NULL);
    rsa = PEM_read_RSAPrivateKey(fp, &rsa,NULL, NULL);
    
    // if(rsa == NULL)
    //     perror( "Failed to create RSA");
    PEM_write_bio_RSA_PUBKEY(bio, rsa);
    PKeyLen = BIO_pending(bio); 
    public_key = calloc(PKeyLen+1,1);
    BIO_read(bio,public_key,PKeyLen);
    fclose(fp);
    BIO_free_all(keybio);
    RSA_free(rsa);
    return public_key;
}
//     // int RSA_sign(int type, const unsigned char *m, unsigned int m_length,
//     //          unsigned char *sigret, unsigned int *siglen, RSA *rsa);
unsigned char* signification(char* msg){
    unsigned char* digest;
    unsigned int siglen;
    unsigned char* sign;
    digest = hashchars(msg);
    RSA *rsa=NULL;
    FILE* fp = fopen("user.key","rb");
    if(fp==NULL)
        perror("user.Key is NOT EXIST");

    rsa = PEM_read_RSAPrivateKey(fp, &rsa,NULL, NULL);

    siglen = RSA_size(rsa);
    sign = malloc(siglen); // 128
    RSA_sign(NID_sha256,digest,SHA256_DIGEST_LENGTH,sign,&siglen,rsa);

    if(fp!=NULL)
        fclose(fp);
    return sign;
}
    //            const unsigned char *sigbuf, unsigned int siglen, RSA *rsa);
int verification(char* msg,unsigned char* sign,RSA* rsa){
    int flag;
    unsigned char* digest;
    int siglen = RSA_size(rsa);
    digest = hashchars(msg);
    flag = RSA_verify(NID_sha256,digest,SHA256_DIGEST_LENGTH,sign,siglen,rsa);
    return flag;
}


// int main(int argc,char* argv[]){
//     RSA* PriRsa;
//     RSA* PuRSA;
//     RSA *fakeKey = RSA_generate_key(1024, 3, 0, 0);
//     unsigned char* sign;
//     char* sig_vis;
//     int flag=0;

//     char* msg = "Hello World!";
//     char* public_key;

//     FILE* fp = fopen("user.key","rb");
//     if(fp==NULL)
//         perror("PK.Key is NOT EXIST");

//     PriRsa = PEM_read_RSAPrivateKey(fp, &PriRsa,NULL, NULL);
//     fclose(fp);
//     public_key = genPublicKey();
    
//     BIO *puBio;
//     puBio = BIO_new_mem_buf(public_key, -1);
//     PuRSA = PEM_read_bio_RSA_PUBKEY(puBio, &PuRSA,NULL,NULL);

//     sign = signification(msg);
    
//     flag = verification(msg,sign,PuRSA);

//     if(flag == 0)
//         fprintf(stdout,"sig failed\n");
//     else{
//         fprintf(stdout,"sig complelte\n");
//     }
//     sig_vis = malloc(RSA_size(PuRSA)<1);
//     for(int i=0 ;i<RSA_size(PuRSA);i++){
//         sprintf(sig_vis+strlen(sig_vis),"%02x",sign[i]);
//     }
//     fprintf(stderr,"%s",sig_vis);

// }
