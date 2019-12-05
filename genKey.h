#include <openssl/rsa.h>
#include <openssl/pem.h>

RSA * createRSA(unsigned char * key,int public);
void genPrivateKey();
char* genPublicKey();


