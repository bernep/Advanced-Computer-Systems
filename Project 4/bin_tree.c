/* bin_tree.c */

/* This code was initially taken from:
   https://stackoverflow.com/questions/26874194/binary-tree-for-strings-c */
/* The code has been modified and re-formatted to suit the purposes of this project */

// Import Libraries
#include "bin_tree.h"

// Function: Insert elements into binary tree
uint tree_insert(char* key, uint index, uint encoded_val, struct TreeNode** leaf) {
    int res;
    // Insert unique element into tree if node is available and initialize all its values
    if( *leaf == NULL ) {
        *leaf = (struct TreeNode*)malloc(sizeof(struct TreeNode));
        (*leaf)->value = malloc(strlen (key) + 1);
        strcpy ((*leaf)->value, key);
        (*leaf)->encoded_value = encoded_val;
        (*leaf)->indices = (struct ListNode*)malloc(sizeof(struct ListNode));
        (*leaf)->indices->index = index;
        (*leaf)->indices->next = NULL;  // Initialize the next pointer to NULL
        (*leaf)->p_left = NULL;
        (*leaf)->p_right = NULL;
    }
    // Otherwise, continue searching or modify duplicate entries
    else {
        res = strcmp(key, (*leaf)->value);
        if(res < 0) {
            return tree_insert(key, index, encoded_val, &(*leaf)->p_left);
        } else if(res > 0) {
            return tree_insert(key, index, encoded_val, &(*leaf)->p_right);
        } else {
            // Add entry index to list
            struct ListNode* ptr = (*leaf)->indices;
            while (ptr->next != NULL) {
                ptr = ptr->next;
            }
            ptr->next = (struct ListNode*)malloc(sizeof(struct ListNode));
            ptr->next->index = index;
            ptr->next->next = NULL;
            return 1;  // indicate duplicate entry
        }

    }
    return 0;  // indicate unique entry
}

// Function: Recursively print out the tree inorder
void tree_print(struct TreeNode* root) {
    if( root != NULL ) {
        tree_print(root->p_left);
        printf("   %d:\t%s\n", root->encoded_value, root->value);
        tree_print(root->p_right);
    }
}

// Function: Search tree for encoded value based on string search term
uint tree_str_search(char* key, struct TreeNode* leaf) {
    int res;
    if( leaf != NULL ) {
        res = strcmp(key, leaf->value);
        if( res < 0) {
            return tree_str_search(key, leaf->p_left);
        } else if( res > 0) {
            return tree_str_search(key, leaf->p_right);
        } else {
            return leaf->encoded_value;	 // return encoded value
	    }
    }
    return 0;  // return invalid encoded value
}

// Function: Search tree for string based on code search term
// ---> Note: This function stores the resulting string in str_res
void tree_code_search(uint code, char* str_res, struct TreeNode* leaf) {
    if( leaf != NULL ) {
        if (code == leaf->encoded_value) {
            strcpy(str_res, leaf->value); // copy string to result buffer
        } else {
            tree_code_search(code, str_res, leaf->p_left);
            tree_code_search(code, str_res, leaf->p_right);
        }
    }
}

// Function: Search tree for strings whose prefix matches the search term
// ---> Note: This function stores the resulting codes in the codes[] array
void tree_prefix_search(const char* prefix, uint* codes,
                        size_t* codes_size, struct TreeNode* leaf) {
    if( leaf != NULL ) {
        if (strncmp(leaf->value, prefix, strlen(prefix)) == 0) {
            codes[*codes_size] = leaf->encoded_value;
            (*codes_size)++;
        }
        tree_prefix_search(prefix, codes, codes_size, leaf->p_left);
        tree_prefix_search(prefix, codes, codes_size, leaf->p_right);
    }  
}

// Function: Get pointer to dictionary entry's listed indices
struct ListNode* tree_get_indices(uint code, struct TreeNode* leaf) {
    if( leaf != NULL ) {
        if (code == leaf->encoded_value) {
            return leaf->indices;
        } else {
            struct ListNode* left_result = tree_get_indices(code, leaf->p_left);
            if (left_result != NULL) {
                return left_result;
            }
            return tree_get_indices(code, leaf->p_right);
        }
    }
    return NULL;
}


// Function: Delete tree
void tree_delete(struct TreeNode** leaf) {
    if (*leaf != NULL) {
        // Recursion
        tree_delete(&(*leaf)->p_left);
        tree_delete(&(*leaf)->p_right);

        // Delete ListNode entries
        struct ListNode* current = (*leaf)->indices;
        struct ListNode* temp;
        while (current != NULL) {
            temp = current->next;
            free(current);
            current = temp;
        }

        // Delete TreeNode
        free((*leaf)->value);
        free(*leaf);
    }
}

