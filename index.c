#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include "index.h"


contents* newIndexContent(indexContent* content){
    contents *temp = (contents*)malloc(sizeof(contents));
    temp->content = content;
    memcpy(temp->content,content,sizeof(indexContent));
    temp->left = temp->right = NULL;
    return temp;
}
contents* indexSearch(contents* root,char* key){
    if(root == NULL || (strcmp(root->content->name, key) ==0))
        return root;
    if(strcmp(root->content->name,key) <0)
        return indexSearch(root->right, key);
    return indexSearch(root->left, key);
}
contents *indexInsert(contents* root,indexContent* content){
    if(root == NULL)
        return newIndexContent(content);
    if(strcmp(content->name, root->content->name) < 0)
        root->left = indexInsert(root->left, content);
    else if(strcmp(content->name, root->content->name) > 0)
        root->right = indexInsert(root->right, content);
    return root;
}

// void saveIndex(contents *root, FILE* index) 
// { 
//     unsigned char dump = 0xFF;
//     if (root == NULL){ 
//         for(int i=0;i<sizeof(indexContent);i++){
//             fwrite(dump,1,1,index);
//         }
//     }
//     // Else, store current node and recur for its children 
//     fwrite(root->content, sizeof(indexContent), 1, index);
//     serialize(root->left, index); 
//     serialize(root->right, index); 
// } 
  
// // This function constructs a tree from a file pointed by 'fp' 
// void deSerialize(Node* &root, FILE* index) 
// { 
//     // Read next item from file. If theere are no more items or next 
//     // item is marker, then return 
//     int val;
//     indexContent* content = (indexContent*)malloc(sizeof(indexContent));
//     if ( !fscanf(fp, "%d ", &val) || val == MARKER) 
//        return; 

//     if( !fread(content, sizeof(indexContent), 1, index) || 
  
//     // Else create node with this item and recur for children 
//     root = newNode(val); 
//     deSerialize(root->left, fp); 
//     deSerialize(root->right, fp); 
// }
// void inorder(Node *root) 
// { 
//     if (root != NULL) 
//     { 
//         inorder(root->left); 
//         printf("%s \n", root->d); 
//         inorder(root->right); 
//     } 
// } 