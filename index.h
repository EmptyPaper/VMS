#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

typedef struct _indexContent{
    char conflict;
    int size;
    char name[__DARWIN_MAXNAMLEN];
    unsigned char hash[64];
}indexContent;

typedef struct _contents{
    struct _contents* left;
    struct _contents* right;
    indexContent* content;
}contents;

contents* newIndexContent(indexContent* content);
contents* indexSearch(contents* root,char* key);
contents* indexInsert(contents* root,indexContent* content);






