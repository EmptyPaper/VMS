#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <zlib.h>
#include <assert.h>
#include "index.h"
void InsertRepairTree(contents* a);
void InsertCase4Step2(contents* n);


contents* GetParent(contents* n) {
  // Note that parent is set to null for the root contents.
  return n == NULL ? NULL : n->parent;
}

contents* GetGrandParent(contents* n) {
  // Note that it will return NULL if this is root or child of root
  return GetParent(GetParent(n));
}

contents* GetSibling(contents* n) {
  contents* p = GetParent(n);

  // No parent means no sibling.
  if (p == NULL) {
    return NULL;
  }

  if (n == p->left) {
    return p->right;
  } else {
    return p->left;
  }
}

contents* GetUncle(contents* n) {
  contents* p = GetParent(n);

  // No parent means no uncle
  return GetSibling(p);
}

void RotateLeft(contents* n) {
  contents* nnew = n->right;
  contents* p = GetParent(n);
  assert(nnew != NULL);  // Since the leaves of a 'R'-'B' tree are empty,
                            // they cannot become internal contentss.
  n->right = nnew->left;
  nnew->left = n;
  n->parent = nnew;
  // Handle other child/parent pointers.
  if (n->right != NULL) {
    n->right->parent = n;
  }

  // Initially n could be the root.
  if (p != NULL) {
    if (n == p->left) {
      p->left = nnew;
    } else if (n == p->right) {
      p->right = nnew;
    }
  }
  nnew->parent = p;
}

void RotateRight(contents* n) {
  contents* nnew = n->left;
  contents* p = GetParent(n);
  assert(nnew != NULL);  // Since the leaves of a 'R'-'B' tree are empty,
                            // they cannot become internal contentss.

  n->left = nnew->right;
  nnew->right = n;
  n->parent = nnew;

  // Handle other child/parent pointers.
  if (n->left != NULL) {
    n->left->parent = n;
  }

  // Initially n could be the root.
  if (p != NULL) {
    if (n == p->left) {
      p->left = nnew;
    } else if (n == p->right) {
      p->right = nnew;
    }
  }
  nnew->parent = p;
}
void InsertCase1(contents* n) {
  if (GetParent(n) == NULL) {
    n->color = 'B';
  }
}
void InsertCase2(contents* n) {
  // Do nothing since tree is still valid.
  return;
}
void InsertCase3(contents* n) {
  n->parent->color = 'B';
  GetUncle(n)->color = 'B';
  GetGrandParent(n)->color = 'R';
  InsertRepairTree(GetGrandParent(n));
}

void InsertCase4(contents* n) {
  contents* p = GetParent(n);
  contents* g = GetGrandParent(n);
  if (n == p->right && p == g->left) {
    RotateLeft(p);
    n = n->left;
  } else if (n == p->left && p == g->right) {
    RotateRight(p);
    n = n->right;
  }
  InsertCase4Step2(n);
}

void InsertCase4Step2(contents* n) {
  contents* p = GetParent(n);
  contents* g = GetGrandParent(n);

  if (n == p->left) {
    RotateRight(g);
  } else {
    RotateLeft(g);
  }
  p->color = 'B';
  g->color = 'R';
}

void InsertRepairTree(contents* n) {
  if (GetParent(n) == NULL) {
    InsertCase1(n);
  } else if (GetParent(n)->color == 'B') {
    InsertCase2(n);
  } else if (GetUncle(n) != NULL && GetUncle(n)->color == 'R') {
    InsertCase3(n);
  } else {
    InsertCase4(n);
  }
}




// contents* newIndexContent(indexContent* content){
//     contents *temp = (contents*)malloc(sizeof(contents));
//     temp->content = content;
//     memcpy(temp->content,content,sizeof(indexContent));
//     temp->left = temp->right = NULL;
//     return temp;
// }
// contents* newIndexContent(indexContent* content){
//     contents* temp = (contents*)malloc(sizeof(contents)); 
//     temp->content = content; 
//     temp->next = NULL; 
//     return temp; 
// }
// indexContent* peek(contents** root) { 
//     return (*root)->content; 
// } 
// void pop(contents** root) 
// { 
//     contents* temp = *root; 
//     (*root) = (*root)->next; 
//     free(temp); 
// } 
// contents* indexSearch(contents* root,char* key){
//     if(root == NULL || (strcmp(root->content->name, key) ==0))
//         return root;
//     if(strcmp(root->content->name,key) <0)
//         return indexSearch(root->right, key);
//     return indexSearch(root->left, key);
// }
// contents *indexInsert(contents* root,indexContent* content){
//     if(root == NULL)
//         return newIndexContent(content);
//     if(strcmp(content->name, root->content->name) < 0)
//         root->left = indexInsert(root->left, content);
//     else if(strcmp(content->name, root->content->name) > 0)
//         root->right = indexInsert(root->right, content);
//     return root;
// // }
// void indexInsert(contents** root,indexContent* content){
//     contents* start = (*root); 
//     // Create new Node 
//     contents* temp = newIndexContent(content); 
//     if(start == NULL){
//         *root = temp;
//         return;
//     }
//     char* rootName = (*root)->content->name;
//     char* newName = content->name;
//     if (strcmp(rootName, newName) > 0) {
//         // Insert New Node before head 
//         temp->next = *root; 
//         (*root) = temp;
//     } 
//     else { 
//         // Traverse the list and find a 
//         // position to insert new node 
//         while ((start->next != NULL) && 
//                (strcmp(start->next->content->name, newName) < 0)){ 
//             start = start->next; 
//         }
//         if(start->next != NULL && !strcmp(start->next->content->name, newName))
//             return;
//         // Either at the ends of the list 
//         // or at requi'R' position 
//         temp->next = start->next; 
//         start->next = temp; 
//     } 
// }
int isEmpty(contents** head) { 
    return (*head) == NULL; 
} 
// Driver code 
void loadIndex(contents** hashs){
    int readByte=0;
    gzFile index;
    char buf[256];
    indexContent* content;

    if((index = gzopen("./.VMS/index", "rb")) == NULL)
        perror("gzopen error");
    while(!gzeof(index)){
        content = malloc(sizeof(indexContent));
        if((readByte = gzread(index, content,sizeof(indexContent))) > 0){
            // fprintf(stderr,"load data  -> %s, %d\n",content->name,readByte);
            // content->name[readByte-] ='\0';
            indexInsert(hashs,content);
        }
        // readByte += gzread(index, &(content->size),sizeof(int));
        // readByte += gzread(index, content->name,sizeof(content->name));
        // readByte += gzread(index, content->hash,sizeof(content->hash));
        // memset(content,0,sizeof(indexContent));
    }
    gzclose(index);
}
// void saveIndex(contents* hashs,gzFile index){
//     if(hashs != NULL){
//         saveIndex(hashs->left,index);
//         if((gzwrite(index, hashs->content,sizeof(indexContent))) == 0){
//             DieWithError("gzwrite error");
//         }
//         else{
//             fprintf(stderr,"\nIndexing -> %s", hashs->content->name);
//         }
//         saveIndex(hashs->right,index);
//     }
// }
void saveIndex(contents* hashs,gzFile* index){
    if(index == NULL){
        perror("gzopen error");
    }
    if (hashs == NULL)
        return;
    saveIndex(hashs->left,index);
    if((gzwrite(*index, hashs->content,sizeof(indexContent))) == 0){
        perror("gzwrite error");
    }
    else{
        fprintf(stderr,"\nIndexing -> %s %c", hashs->content->name,hashs->color);
    }
    saveIndex(hashs->right,index);
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
// Left Rotation
void LeftRotate(contents **root,contents *x){
    if (!x || !x->right)
        return ;
    //y sto'R' pointer of right child of x
    contents *y = x->right;

    //store y's left subtree's pointer as x's right child
    x->right = y->left;

    //update parent pointer of x's right
    if (x->right != NULL)
        x->right->parent = x;

    //update y's parent pointer
    y->parent = x->parent;

    // if x's parent is null make y as root of tree
    if (x->parent == NULL)
        (*root) = y;

    // store y at the place of x
    else if (x == x->parent->left)
        x->parent->left = y;
    else    x->parent->right = y;

    // make x as left child of y
    y->left = x;

    //update parent pointer of x
    x->parent = y;
}
// Right Rotation (Similar to LeftRotate)
void rightRotate(contents **root,contents *y){
    if (!y || !y->left)
        return ;
    contents *x = y->left;
    y->left = x->right;
    if (x->right != NULL)
        x->right->parent = y;
    x->parent =y->parent;
    if (x->parent == NULL)
        (*root) = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else y->parent->right = x;
    x->right = y;
    y->parent = x;
}


void insertFixUp(contents **root,contents *z){
    // iterate until z is not the root and z's parent color is 'R'
  while (z != *root && z != (*root)->left && z != (*root)->right && z->parent->color == 'R')
  {
      contents *y;
      // Find uncle and store uncle in y
      if (z->parent && z->parent->parent && z->parent == z->parent->parent->left)
          y = z->parent->parent->right;
      else
          y = z->parent->parent->left;

      // If uncle is 'R', do following
      // (i)  Change color of parent and uncle as 'B'
      // (ii) Change color of grandparent as 'R'
      // (iii) Move z to grandparent
      // if (!y)
      //     z = z->parent->parent;
      if (y!=NULL && y->color == 'R')
      {
          y->color = 'B';
          z->parent->color = 'B';
          z->parent->parent->color = 'R';
          z = z->parent->parent;
      }
      // Uncle is 'B', there are four cases (LL, LR, RL and RR)
      else
      {
          // Left-Left (LL) case, do following
          // (i)  Swap color of parent and grandparent
          // (ii) Right Rotate Grandparent
          if (z->parent == z->parent->parent->left &&
              z == z->parent->left)
          {
              char ch = z->parent->color ;
              z->parent->color = z->parent->parent->color;
              z->parent->parent->color = ch;
              rightRotate(root,z->parent->parent);
          }

          // Left-Right (LR) case, do following
          // (i)  Swap color of current node  and grandparent
          // (ii) Left Rotate Parent
          // (iii) Right Rotate Grand Parent
          if (z->parent && z->parent->parent && z->parent == z->parent->parent->left &&
              z == z->parent->right)
          {
              char ch = z->color ;
              z->color = z->parent->parent->color;
              z->parent->parent->color = ch;
              LeftRotate(root,z->parent);
              rightRotate(root,z->parent->parent);
          }

          // Right-Right (RR) case, do following
          // (i)  Swap color of parent and grandparent
          // (ii) Left Rotate Grandparent
          if (z->parent && z->parent->parent &&
              z->parent == z->parent->parent->right &&
              z == z->parent->right)
          {
              char ch = z->parent->color ;
              z->parent->color = z->parent->parent->color;
              z->parent->parent->color = ch;
              LeftRotate(root,z->parent->parent);
          }

          // Right-Left (RL) case, do following
          // (i)  Swap color of current node  and grandparent
          // (ii) Right Rotate Parent
          // (iii) Left Rotate Grand Parent
          if (z->parent && z->parent->parent && z->parent == z->parent->parent->right &&
              z == z->parent->left)
          {
              char ch = z->color ;
              z->color = z->parent->parent->color;
              z->parent->parent->color = ch;
              rightRotate(root,z->parent);
              LeftRotate(root,z->parent->parent);
          }
      }
  }
  (*root)->color = 'B'; //keep root always 'B'
}

// Utility function to insert newly node in 'R''B' tree
void indexInsert(contents **root, indexContent* content){
  // Allocate memory for new node
  contents *z = (contents*)malloc(sizeof(contents));
  z->content = content;
  z->left = z->right = z->parent = NULL;


    //if root is null make z as root
  if (*root == NULL){
      z->color = 'B';
      (*root) = z;
  }
  else{
    contents *y = NULL;
    contents *x = (*root);

    char *dirOffset = strrchr(z->content->name,'/');
    char *offset;
    unsigned int a;
    unsigned int b;
    a = dirOffset - z->content->name;
    // Follow standard BST insert steps to first insert the node
    while (x != NULL){
        y = x;
        offset = strrchr(x->content->name,'/');
        b = offset - x->content->name;
        if(b < a){
          x = x->left;
        }
        else if(b > a){
          x = x->right;
        }
        else{
          if(!strcmp(z->content->name,x->content->name)){
            free(x->content);
            x->content = content;
            return;
          }
          else if (strcmp(z->content->name,x->content->name) < 0 )
              x = x->left;
          else
              x = x->right;
        }
    }
    z->parent = y;
    if(!strcmp(z->content->name,y->content->name)){
        free(y->content);
        y->content = content;
        return;
    }
    else if (strcmp(z->content->name, y->content->name) > 0)
        y->right = z;
    else
        y->left = z;
    z->color = 'R';

    // call insertFixUp to fix reb-'B' tree's property if it
    // is voilated due to insertion.
    insertFixUp(root,z);
    // InsertRepairTree(z);
  }
}

void push(struct container **contain, contents* hashs){
    struct container* head = *contain;
    struct container* temp = malloc(sizeof(struct container));
    if(head == NULL)
        head = malloc(sizeof(struct container));
    temp->hashs = hashs;
    temp->next = head;
    (*contain) = temp;
}
contents* pop(struct container **contain){
    struct container* head = *contain;
    contents* r = head->hashs;
    *contain = (*contain)->next;
    free(head);
    return r;
}

// A utility function to traverse 'R'-'B' tree in inorder fashion
// void inorder(contents *root)
// {
//     static int last = 0;
//     if (root == NULL)
//         return;
//     inorder(root->left);
//     printf("%d ", root->data);
//     if (root->data < last)
//         printf("\nPUTE\n");
//     last = root->data;
//     inorder(root->right);
// }
