
#define BUFSIZE 1024

char* hashToString(unsigned char* hash);
void* hashFile(char *path);
void *hashStrings(void **string);
void *hashchars(void* string);
int Base64Decode(char* b64message, unsigned char** buffer, size_t* length);
int Base64Encode(const unsigned char* buffer, size_t length, char** b64text);

