/* bin_tree.c */

/* This code was initially taken from https://stackoverflow.com/questions/26874194/binary-tree-for-strings-c */
/* The code has been modified and re-formatted to suit the purposes of this project */

// Import Libraries
#include "bin_tree.h"

// Function: Insert elements into B-tree
int tree_insert(char* key, uint encoded_val, struct TreeNode** leaf, TreeCompare cmp) {
    int res;
    if( *leaf == NULL ) {
        *leaf = (struct TreeNode*) malloc( sizeof( struct TreeNode ) );
        (*leaf)->value = malloc( strlen (key) + 1);     // memory for key
        strcpy ((*leaf)->value, key);                   // copy the key
        (*leaf)->encoded_value = encoded_val;           // assign encoded value to node
        (*leaf)->p_left = NULL;
        (*leaf)->p_right = NULL;
    } else {
        res = cmp (key, (*leaf)->value);
        if(res < 0) {
            return tree_insert(key, encoded_val, &(*leaf)->p_left, cmp);
        } else if(res > 0) {
            return tree_insert(key, encoded_val, &(*leaf)->p_right, cmp);
        } else {                                        // key already exists
            return 1;  // indicate duplicate entry
        }
    }
    return 0;  // indicate unique entry
}

// Function: Compare value of the new node against previous node
int tree_cmp_str(const char* a, const char* b) {
    return (strcmp (a, b));
}

// Function: Recursively print out the tree inorder
void tree_print(struct TreeNode* root) {
    if( root != NULL ) {
        tree_print(root->p_left);
        printf("   %d:\t%s\n", root->encoded_value, root->value);
        tree_print(root->p_right);
    }
}

// Function: Search tree for specified element
uint tree_search(char* key, struct TreeNode* leaf, TreeCompare cmp)  {
    int res;
    if( leaf != NULL ) {
        res = cmp(key, leaf->value);
        if( res < 0) {
            return tree_search(key, leaf->p_left, cmp);
        } else if( res > 0) {
            return tree_search(key, leaf->p_right, cmp);
        } else {
            return leaf->encoded_value;	 // return value encoded value
	}
    }
    return 0;  // return invalid encoded value
}

// Function: Delete tree
void tree_delete(struct TreeNode** leaf) {
    if( *leaf != NULL ) {
        tree_delete(&(*leaf)->p_left);
        tree_delete(&(*leaf)->p_right);
        free( (*leaf)->value );
        free( (*leaf) );
    }
}
