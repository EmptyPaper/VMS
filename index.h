#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

typedef struct _indexContent{
    char conflict;
    int size;
    char name[__DARWIN_MAXNAMLEN];
    unsigned char hash[32];
}indexContent;

// typedef struct _contents{
//     struct _contents* left;
//     struct _contents* right;
//     struct _contents* next;
//     indexContent* content;
// }contents;
typedef struct _contents{
    char color;
    struct _contents* left;
    struct _contents* right;
    struct _contents* parent;
    indexContent* content;
}contents;
struct container{
    char rootDir[255];
    contents* hashs;
    struct container* next;
};

contents* newIndexContent(indexContent* content);
// contents* indexSearch(contents* root,char* key);
void indexInsert(contents** root,indexContent* content);
void loadIndex(contents** hashs);
void saveIndex(contents* hashs,gzFile* index);
void push(struct container **contain, contents* hashs);
contents* pop(struct container **contain);









