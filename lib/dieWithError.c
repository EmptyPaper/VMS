#include <stdio.h>

void dieWithError(char* msg){
    fprintf(stderr,"%s",msg);
    exit(1);
}