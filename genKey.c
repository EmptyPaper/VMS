#include <stdio.h>
#include <stdlib.h>
#include "genKey.h"

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
    FILE *fp = fopen("PK.key","w");
    if(fp == NULL)
        perror("file open error");
    
    /* To get the C-string PEM form: */
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPrivateKey(bio, rsa, NULL, NULL, 0, NULL, NULL);
    keylen = BIO_pending(bio);
    pem_key = calloc(keylen+1, 1); /* Null-terminate */
    BIO_read(bio, pem_key, keylen);
    printf("%s", pem_key);

    
    fprintf(fp,"%s",pem_key);
    fclose(fp);
    BIO_free_all(bio);
    RSA_free(rsa);
    free(pem_key);
}
char* genPublicKey(){
    int PKeyLen;
    char *privateKey = (char*)malloc(sizeof(char)*887);
    char *public_key;
    FILE* fp;
    RSA *rsa= NULL;
    BIO *keybio;
    BIO *bio = BIO_new(BIO_s_mem());
    fp = fopen("PK.key","r");
    if(fp==NULL)
        perror("PK.Key is NOT EXIST");

    fread(privateKey,1,887,fp);
    keybio = BIO_new_mem_buf(privateKey, -1);
    if (keybio==NULL)
        perror( "Failed to create key BIO");
    // if(public)
    // {
    //     rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
    // }
    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
    if(rsa == NULL)
        perror( "Failed to create RSA");
    PEM_write_bio_RSA_PUBKEY(bio, rsa);
    PKeyLen = BIO_pending(bio);
    public_key = calloc(PKeyLen+1,1);
    BIO_read(bio,public_key,PKeyLen);

    fclose(fp);
    BIO_free_all(keybio);
    RSA_free(rsa);
    return public_key;
}

// void signation(){
//     // int RSA_sign(int type, const unsigned char *m, unsigned int m_length,
//     //          unsigned char *sigret, unsigned int *siglen, RSA *rsa);
//     RSA_sign(NID_sha256, digest, digest_len, &sign_buffer, sign_len, rsa_key);

//     // int RSA_verify(int type, const unsigned char *m, unsigned int m_length,
//     //            const unsigned char *sigbuf, unsigned int siglen, RSA *rsa);
// }
