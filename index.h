#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#define RED 0
#define BLACK 1

typedef struct _indexContent{
    char conflict;
    int size;
    char type[5];
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
    contents* hashs;
    struct container* next;
};

// contents* indexSearch(contents* root,char* key);

void loadIndex();
void saveIndex(contents* hashs,gzFile* index);

void push(struct container **contain, contents* hashs);
contents* pop(struct container **contain);
unsigned countDepth(char* path);


void left_rotate(contents *x);
void right_rotate(contents *x);
void tree_print(contents *x);
void red_black_insert(indexContent *x);
void red_black_insert_fixup(contents *z);
contents *tree_search(indexContent *x);
contents *tree_minimum(contents *x);
void red_black_transplant(contents *u, contents *v);
void red_black_delete(contents *z);
void red_black_delete_fixup(contents *x);

contents* NILL;
contents* ROOT;

unsigned int PUSHCOUNT;












