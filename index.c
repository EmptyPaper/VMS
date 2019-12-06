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


/* Global, since all function will access them */



/* Print tree keys by inorder tree walk */

void tree_print(contents *x){
	if(x != NILL){
		tree_print(x->left);
		printf("%s\t", x->content->name);
		tree_print(x->right);
	}
}

// contents *tree_search(indexContent* key){
// 	contents *x;

// 	x = ROOT;
// 	while(x != NILL && x->key != key){
// 		if(key < x->key){
// 			x = x->left;
// 		}
// 		else{
// 			x = x->right;
// 		}
// 	}

// 	return x;
// }

contents *tree_minimum(contents *x){
	while(x->left != NILL){
		x = x->left;
	}
	return x;
}

/*
 * Insertion is done by the same procedure for BST Insert. Except new node is colored
 * RED. As it is coloured RED it may violate property 2 or 4. For this reason an
 * auxilary procedure called red_black_insert_fixup is called to fix these violation.
 */

void red_black_insert(indexContent* content){
	contents *z; 
	contents *x;
	contents *y;
	z = malloc(sizeof(contents));

	z->content = content;
	z->color = RED;
	z->left = NILL;
	z->right = NILL;

	x = ROOT;
	y = NILL;

    int depth = countDepth(content->name);
    int tempDepth;
	/* 
	 * Go through the tree untill a leaf(NILL) is reached. y is used for keeping
	 * track of the last non-NILL node which will be z's parent.
	 */
	while(x != NILL){
		y = x;
        tempDepth = countDepth(x->content->name);
		if(depth > tempDepth){
			x = x->left;
		}
		else if(depth < tempDepth){
			x = x->right;
		}
		else{
			if(strcmp(x->content->name,z->content->name) < 0)
				x = x->left;
			else if(strcmp(x->content->name,z->content->name) > 0)
				x = x->right;
			else if(!strcmp(x->content->name,z->content->name)){
				free(x->content);
				x->content = z->content;
				return;
			}
		}
	}
	if(y == NILL){
		ROOT = z;
	}
	if(depth > tempDepth){
		y->left = z;
	}
	else if(depth < tempDepth){
		y->right = z;
	}
	else{
		if(strcmp(y->content->name,z->content->name) < 0)
			y->left = z;
		else if(strcmp(y->content->name,z->content->name) > 0)
			y->right = z;
	}
	z->parent = y;

	red_black_insert_fixup(z);
}

/*
 * Here is the psudocode for fixing violations.
 * 
 * while (z's parent is RED)
 *		if(z's parent is z's grand parent's left child) then
 *			if(z's right uncle or grand parent's right child is RED) then
 *				make z's parent and uncle BLACK
 *				make z's grand parent RED
 *				make z's grand parent new z as it may violate property 2 & 4
 *				(so while loop will contineue)
 *			
 *			else(z's right uncle is not RED)
 *				if(z is z's parents right child) then
 *					make z's parent z
 *					left rotate z
 *				make z's parent's color BLACK
 *				make z's grand parent's color RED
 *				right rotate z's grand parent
 *				( while loop won't pass next iteration as no violation)
 *
 *		else(z's parent is z's grand parent's right child)
 *			do exact same thing above just swap left with right and vice-varsa
 *
 * At this point only property 2 can be violated so make root BLACK
 */

void red_black_insert_fixup(contents *z){
	while(z->parent->color == RED){

		/* z's parent is left child of z's grand parent*/
		if(z->parent == z->parent->parent->left){

			/* z's grand parent's right child is RED */
			if(z->parent->parent->right->color == RED){
				z->parent->color = BLACK;
				z->parent->parent->right->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			}

			/* z's grand parent's right child is not RED */
			else{
				
				/* z is z's parent's right child */
				if(z == z->parent->right){
					z = z->parent;
					left_rotate(z);
				}

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				right_rotate(z->parent->parent);
			}
		}

		/* z's parent is z's grand parent's right child */
		else{
			
			/* z's left uncle or z's grand parent's left child is also RED */
			if(z->parent->parent->left->color == RED){
				z->parent->color = BLACK;
				z->parent->parent->left->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			}

			/* z's left uncle is not RED */
			else{
				/* z is z's parents left child */
				if(z == z->parent->left){
					z = z->parent;
					right_rotate(z);
				}
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				left_rotate(z->parent->parent);
			}
		}
	}

	ROOT->color = BLACK;
}

/*
 * Lets say y is x's right child. Left rotate x by making y, x's parent and x, y's
 * left child. y's left child becomes x's right child.
 * 
 * 		x									y
 *	   / \                                 / \
 *  STA   y			----------->		  x   STC
 *		 / \							 / \
 *	  STB   STC						  STA   STB
 */

void left_rotate(contents *x){
	contents *y;
	y = x->right;
	if(x==NILL || x->right==NILL){
		return;
	}
	/* Make y's left child x's right child */
	x->right = y->left;
	if(y->left != NILL){
		y->left->parent = x;
	}

	/* Make x's parent y's parent and y, x's parent's child */
	y->parent = x->parent;
	if(y->parent == NILL){
		ROOT = y;
	}
	else if(x == x->parent->left){
		x->parent->left = y;
	}
	else{
		x->parent->right = y;
	}
	
	/* Make x, y's left child & y, x's parent */
	y->left = x;
	x->parent = y;
}

/*
 * Lets say y is x's left child. Right rotate x by making x, y's right child and y
 * x's parent. y's right child becomes x's left child.
 *
 *			|											|
 *			x											y
 *		   / \										   / \
 *		  y   STA		---------------->			STB	  x
 *		 / \											 / \
 *	  STB   STC										  STC   STA
 */

void right_rotate(contents *x){
	contents *y;

	y = x->left;
	/* Make y's right child x's left child */
	if(x== NILL || x->left){
		return;
	}
	x->left = y->right;

	if(y->right != NILL){
		y->right->parent = x;
	}

	/* Make x's parent y's parent and y, x's parent's child */
	y->parent = x->parent;
	if(y->parent == NILL){
		ROOT = y;
	}
	else if(x == x->parent->left){
		x->parent->left = y;	
	}
	else{
		x->parent->right = y;
	}

	/* Make y, x's parent and x, y's child */
	y->right = x;
	x->parent = y;

}

/*
 * Deletion is done by the same mechanism as BST deletion. If z has no child, z is
 * removed. If z has single child, z is replaced by its child. Else z is replaced by
 * its successor. If successor is not z's own child, successor is replaced by its
 * own child first. then z is replaced by the successor.
 *
 * A pointer y is used to keep track. In first two case y is z. 3rd case y is z's
 * successor. So in first two case y is removed. In 3rd case y is moved.
 *
 *Another pointer x is used to keep track of the node which replace y.
 * 
 * As removing or moving y can harm red-black tree properties a variable
 * yOriginalColor is used to keep track of the original colour. If its BLACK then
 * removing or moving y harm red-black tree properties. In that case an auxilary
 * procedure red_black_delete_fixup(x) is called to recover this.
 */

void red_black_delete(contents *z){
	contents *y, *x;
	char yOriginalColor;

	y = z;
	yOriginalColor = y->color;
	if(z->left == NILL){
		x = z->right;
		red_black_transplant(z, z->right);
	}
	else if(z->right == NILL){
		x = z->left;
		red_black_transplant(z, z->left);
	}
	else{
		y = tree_minimum(z->right);
		yOriginalColor = y->color;

		x = y->right;

		if(y->parent == z){
			x->parent = y;
		}
		else{
			red_black_transplant(y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}

		red_black_transplant(z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}
	if(yOriginalColor == BLACK){
		red_black_delete_fixup(x);
	}
}

/*
 * As y was black and removed x gains y's extra blackness.
 * Move the extra blackness of x until
 *		1. x becomes root. In that case just remove extra blackness
 *		2. x becomes a RED and BLACK node. in that case just make x BLACK
 *
 * First check if x is x's parents left or right child. Say x is left child
 *
 * There are 4 cases.
 *
 * Case 1: x's sibling w is red. transform case 1 into case 2 by recoloring
 * w and x's parent. Then left rotate x's parent.
 *
 * Case 2: x's sibling w is black, w's both children is black. Move x and w's
 * blackness to x's parent by coloring w to RED and x's parent to BLACK.
 * Make x's parent new x.Notice if case 2 come through case 1 x's parent becomes 
 * RED and BLACK as it became RED in case 1. So loop will stop in next iteration.
 *
 * Case 3: w is black, w's left child is red and right child is black. Transform
 * case 3 into case 4 by recoloring w and w's left child, then right rotate w.
 *
 * Case 4: w is black, w's right child is red. recolor w with x's parent's color.
 * make x's parent BLACK, w's right child black. Now left rotate x's parent. Make x
 * point to root. So loop will be stopped in next iteration.
 *
 * If x is right child of it's parent do exact same thing swapping left<->right
 */

void red_black_delete_fixup(contents *x){
	contents *w;
	while(x != ROOT && x->color == BLACK){
		// fprintf(stderr,"dbg");
		if(x == x->parent->left){
			w = x->parent->right;
			if(w->color == RED){
				w->color = BLACK;
				x->parent->color = RED;
				left_rotate(x->parent);
				w = x->parent->right;
			}
			if(w->left->color == BLACK && w->right->color == BLACK){
				w->color = RED;
				x->parent->color = BLACK;
				x = x->parent;
			}
			else{
				if(w->right->color == BLACK){
					w->color = RED;
					w->left->color = BLACK;
					right_rotate(w);
					w = x->parent->right;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				x->right->color = BLACK;
				left_rotate(x->parent);
				x = ROOT;		

			}

		}
		else{
			w = x->parent->left;

			if(w->color == RED){
				w->color = BLACK;
				x->parent->color = BLACK;
				right_rotate(x->parent);
				w = x->parent->left;
			}
			
			if(w->left->color == BLACK && w->right->color == BLACK){
				w->color = RED;
				x->parent->color = BLACK;
				x = x->parent;

			}
			else{			

				if(w->left->color == BLACK){
					w->color = RED;
					w->right->color = BLACK;
					left_rotate(w);
					w = x->parent->left;

				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				right_rotate(x->parent);
				x = ROOT;

			}
		}

	}
	x->color = BLACK;
}

/* replace node u with node v */
void red_black_transplant(contents *u, contents *v){
	if(u->parent == NILL){
		ROOT = v;
	}
	else if(u == u->parent->left){
		u->parent->left = v;
	}
	else{
		u->parent->right = v;
	}
	v->parent = u->parent;
}

// Driver code 
void loadIndex(){
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
            red_black_insert(content);
        }

    }
    gzclose(index);
}

void saveIndex(contents* hashs,gzFile* index){
    if(index == NULL){
        perror("gzopen error");
    }
    if (hashs == NILL)
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

// // Left Rotation
// void LeftRotate(contents **root,contents *x){
//     if (!x || !x->right)
//         return ;
//     //y sto'R' pointer of right child of x
//     contents *y = x->right;

//     //store y's left subtree's pointer as x's right child
//     x->right = y->left;

//     //update parent pointer of x's right
//     if (x->right != NILL)
//         x->right->parent = x;

//     //update y's parent pointer
//     y->parent = x->parent;

//     // if x's parent is null make y as root of tree
//     if (x->parent == NILL)
//         (*root) = y;

//     // store y at the place of x
//     else if (x == x->parent->left)
//         x->parent->left = y;
//     else    x->parent->right = y;

//     // make x as left child of y
//     y->left = x;

//     //update parent pointer of x
//     x->parent = y;
// }
// // Right Rotation (Similar to LeftRotate)
// void rightRotate(contents **root,contents *y){
//     if (!y || !y->left)
//         return ;
//     contents *x = y->left;
//     y->left = x->right;
//     if (x->right != NILL)
//         x->right->parent = y;
//     x->parent =y->parent;
//     if (x->parent == NILL)
//         (*root) = x;
//     else if (y == y->parent->left)
//         y->parent->left = x;
//     else y->parent->right = x;
//     x->right = y;
//     y->parent = x;
// }


// void insertFixUp(contents **root,contents *z){
//     // iterate until z is not the root and z's parent color is 'R'
//   while (z != *root && z != (*root)->left && z != (*root)->right && z->parent->color == 'R')
//   {
//       contents *y;
//       // Find uncle and store uncle in y
//       if (z->parent && z->parent->parent && z->parent == z->parent->parent->left)
//           y = z->parent->parent->right;
//       else
//           y = z->parent->parent->left;

//       // If uncle is 'R', do following
//       // (i)  Change color of parent and uncle as 'B'
//       // (ii) Change color of grandparent as 'R'
//       // (iii) Move z to grandparent
//     //    if (!y)
//       //     z = z->parent->parent;
//       if(y==NULL)
//         break;
//       if (y->color == 'R')
//       {
//           y->color = 'B';
//           z->parent->color = 'B';
//           z->parent->parent->color = 'R';
//           z = z->parent->parent;
//       }
//       // Uncle is 'B', there are four cases (LL, LR, RL and RR)
//       else
//       {
//           // Left-Left (LL) case, do following
//           // (i)  Swap color of parent and grandparent
//           // (ii) Right Rotate Grandparent
//           if (z->parent == z->parent->parent->left &&
//               z == z->parent->left)
//           {
//               char ch = z->parent->color ;
//               z->parent->color = z->parent->parent->color;
//               z->parent->parent->color = ch;
//               rightRotate(root,z->parent->parent);
//           }

//           // Left-Right (LR) case, do following
//           // (i)  Swap color of current node  and grandparent
//           // (ii) Left Rotate Parent
//           // (iii) Right Rotate Grand Parent
//           if (z->parent && z->parent->parent && z->parent == z->parent->parent->left &&
//               z == z->parent->right)
//           {
//               char ch = z->color ;
//               z->color = z->parent->parent->color;
//               z->parent->parent->color = ch;
//               LeftRotate(root,z->parent);
//               rightRotate(root,z->parent->parent);
//           }

//           // Right-Right (RR) case, do following
//           // (i)  Swap color of parent and grandparent
//           // (ii) Left Rotate Grandparent
//           if (z->parent && z->parent->parent &&
//               z->parent == z->parent->parent->right &&
//               z == z->parent->right)
//           {
//               char ch = z->parent->color ;
//               z->parent->color = z->parent->parent->color;
//               z->parent->parent->color = ch;
//               LeftRotate(root,z->parent->parent);
//           }

//           // Right-Left (RL) case, do following
//           // (i)  Swap color of current node  and grandparent
//           // (ii) Right Rotate Parent
//           // (iii) Left Rotate Grand Parent
//           if (z->parent && z->parent->parent && z->parent == z->parent->parent->right &&
//               z == z->parent->left)
//           {
//               char ch = z->color ;
//               z->color = z->parent->parent->color;
//               z->parent->parent->color = ch;
//               rightRotate(root,z->parent);
//               LeftRotate(root,z->parent->parent);
//           }
//       }
//   }
//   (*root)->color = 'B'; //keep root always 'B'
// }

unsigned countDepth(char* path){
  int len = strlen(path);
  unsigned int count=0;
  for(int i = 0 ; i<len;i++){
    if(path[i] == '/')
      count++;
  }
  return count;
}

// void indexInsert(contents **root, indexContent* content){
//   // Allocate memory for new node
//   contents *z = (contents*)malloc(sizeof(contents));
//   z->content = content;
//   z->left = z->right = z->parent = NILL;


//     //if root is null make z as root
//   if (*root == NILL){
//       z->color = 'B';
//       (*root) = z;
//   }
//   else{
//     contents *y = NILL;
//     contents *x = (*root);

//     // char *dirOffset = strrchr(z->content->name,'/');
//     // char *offset;
//     unsigned int a;
//     unsigned int b;
//     // a = dirOffset - z->content->name;
//     a = countDepth(z->content->name);
//     // Follow standard BST insert steps to first insert the node
//     while (x != NILL){
//         y = x;
//         // offset = strrchr(x->content->name,'/');
//         // b = offset - x->content->name;
//         // b =countDepth(x->content->name);
//         if(!strcmp(x->content->name,z->content->name)){
//           free(x->content);
//           x->content = content;
//           return;
//         }
//         else if(strcmp(x->content->name, z->content->name) > 0)
//           x = x->left;
//         else if(strcmp(x->content->name, z->content->name) < 0)
//           x = x->right;
//         // if(b < a){
//         //   x = x->left;
//         // }
//         // else if(b > a){
//         //   x = x->right;
//         // }
//         // else{
//         //   if(!strcmp(z->content->name,x->content->name)){
//         //     free(x->content);
//         //     x->content = content;
//         //     return;
//         //   }
//         //   else if (strcmp(z->content->name,x->content->name) < 0 )
//         //       x = x->left;
//         //   else
//         //       x = x->right;
//         // }
//     }
//     z->parent = y;
//     // if(!strcmp(z->content->name,y->content->name)){
//     //     free(y->content);
//     //     y->content = content;
//     //     return;
//     // }
//     if(strcmp(y->content->name, z->content->name) > 0)
//       y->left = z;
//     else{
//       y->right = z;
//     }
//     // if (b > a)
//     //     y->right = z;
//     // else if(b< a)
//     //     y->left = z;
//     // else{
//     //   if (strcmp(z->content->name,y->content->name) < 0 )
//     //     y->left = z;
//     //   else
//     //     y->right = z;
//     // }
//     z->color = 'R';

//     // call insertFixUp to fix reb-'B' tree's property if it
//     // is voilated due to insertion.
//     insertFixUp(root,z);
//     // InsertRepairTree(z);
//   }
// }

void push(struct container **contain, contents* hashs){
    struct container* head = *contain;
    struct container* temp = malloc(sizeof(struct container));
    
    temp->hashs = hashs;
    temp->next = head;
    (*contain) = temp;
	PUSHCOUNT++;
}
contents* pop(struct container **contain){
    struct container* head = *contain;
	if(*contain == NULL){
		return NULL;
	}
    contents* r = head->hashs;
    *contain = (*contain)->next;
    free(head);
	PUSHCOUNT--;
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
