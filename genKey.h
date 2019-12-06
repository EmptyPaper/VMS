#include <openssl/rsa.h>
#include <openssl/pem.h>

RSA * createRSA(unsigned char * key,int public);
void genPrivateKey();
char* genPublicKey();
unsigned char* signification(char* msg);
int verification(char* msg,unsigned char* sign,RSA* rsa);




