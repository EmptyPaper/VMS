#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

// void create_dir(){
//     #ifdef __linux__
//         mkdir(".vms",777);
//     #else
//         _mkdir(".vms");
//     #endif
// }




void add(int argc,char* argv[]){
    struct dirent* ent;
    DIR *dir = opendir("./");
    FILE *fp;
    FILE *fIndex;
    unsigned int fileSize;
    unsigned char zero = 0x0;
    
    // if(checkRepository()){
    //     fIndex = fopen("./.VMS/index","wb");
    //     while((ent = readdir(dir)) != NULL){
    //         fp = fopen(ent->d_name,"rb");
    //         fileSize =  checkFileSize(fp);
    //         fwrite(ent->d_name,sizeof(ent->d_name), 1,fIndex);
    //         fwrite(&zero,1, 1,fIndex);
    //         fwrite(&fileSize,sizeof(fileSize), 1,fIndex);
    //         fclose(fp);
    //     }
    if(checkRepository()){
        fIndex = fopen("./.VMS/index\n","wb");
        struct fileInfo fInfo;
        for(int i = 1; i < argc;i++){
           if((0 != strcmp(argv[i],".")) || (strcmp(argv[i], "..") !=0 )){
                fprintf(stderr,"%s is added",argv[i]);
                fp = fopen(argv[i],"rb");
                fileSize = checkFileSize(fp);
                strcpy(fInfo.name,argv[i]);
                fInfo.size = fileSize;
                fwrite(&fInfo,sizeof(fInfo),1,fIndex);
                fclose(fp);
                memset(fInfo.name, 0,30);
           }
        }
    }
    fclose(fIndex);
}

int main(int argc,char* argv[]){
    add(argc,argv);
    return 0;
}
