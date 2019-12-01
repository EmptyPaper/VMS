#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

void catFile(char* fileName){
    gzFile gf;
    char buf[256];
    int byteRead;
    gf = gzopen(fileName,"r");
    while(!gzeof(gf)){
        byteRead = gzread(gf,buf,256);
        fwrite(buf,1,byteRead,stdout);
    }
    // while(gzread(gf,buf,256) > 0){
    //     fprintf(stderr,"%s",buf);
    // }
}

int main(int argc,char* argv[]){
    if(argc != 2){
        fprintf(stderr, "USAGE : ~~~");
        return 0;
    }
    catFile(argv[1]);
}