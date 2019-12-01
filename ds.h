#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef FILE_NAME_SIZE
    #define FILE_NAME_SIZE 64
#endif

void* hashFile(char* path);
void *hashStrings(void **string);
void *hashchars(void* string);
char* hashToString(unsigned char* hash);

// /* return dest offset of content*/
// int findIndex(struct indexContent* content, int l, int r, char* key){
//     int mid = 1+(r+l)/2;
//     int out = -1;
//     if(r<=l){
//         if(!strcmp(content[mid].name, key))
//             out = mid;
//         else if(strcmp(content[mid].name, key) < 0)
//             out = findIndex(content, mid+1, r,key);
//         else if(strcmp(content[mid].name, key) > 0)
//             out = findIndex(content, l, mid-1, key);
//     }
//     return out;
// }

unsigned int checkFileSize(FILE* fp){
    unsigned int fileSize;
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return fileSize;
}

// int isFile(const char* path){
//     struct stat path_stat;
//     stat(path, &path_stat);

//     return S_ISREG(path_stat.st_mode);
// }
// // A utility function to create a new BST node 
// Node *newNode(void* item) 
// { 
//     Node *temp =  (Node *)malloc(sizeof(Node)); 
//     temp->d = item; 
//     temp->left = temp->right = NULL; 
//     return temp; 
// } 
   
// // A utility function to do inorder traversal of BST 
// void inorder(Node *root) 
// { 
//     if (root != NULL) 
//     { 
//         inorder(root->left); 
//         printf("%s \n", root->d); 
//         inorder(root->right); 
//     } 
// } 
// Node* search(Node* root,char* key) 
// { 
//     // Base Cases: root is null or key is present at root 
//     if (root == NULL || (strcmp(root->d->name, key) == 0)) 
//        return root; 
     
//     // Key is greater than root's key 
//     if (strcmp(root->d->name,key) < 0) 
//        return search(root->right, key); 
  
//     // Key is smaller than root's key 
//     return search(root->left, key); 
// } 

// Node* insert(Node* node, struct fileInfo* fi) 
// { 
//     /* If the tree is empty, return a new node */
//     if (node == NULL) return newNode(fi); 
  
//     /* Otherwise, recur down the tree */
//     if (strcmp(fi->name,node->d->name) < 0) 
//         node->left  = insert(node->left, fi); 
//     else if (strcmp(fi->name,node->d->name) > 0) 
//         node->right = insert(node->right, fi);

//     /* return the (unchanged) node pointer */
//     return node; 
// }