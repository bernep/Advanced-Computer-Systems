/* bin_tree.h */

// Import libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Definitions
#define MAX_STR_LEN 100  // maximum string length
typedef unsigned int uint;  // for convenience
typedef int (*TreeCompare)(const char*, const char*);  // for convenience

// Binary tree Node struct
struct TreeNode {
    char* value;
    uint encoded_value;
    uint num_children;
    struct TreeNode* p_left;
    struct TreeNode* p_right;
};

// Function prototypes
int tree_insert(char* key, uint encoded_val, struct TreeNode** leaf);
void tree_print(struct TreeNode* root);
uint tree_str_search(char* key, struct TreeNode* leaf);
void tree_code_search(uint code, char* str_res, struct TreeNode* leaf);
void tree_prefix_search(const char* prefix, uint* codes,
                        size_t* codes_size, struct TreeNode* leaf);
void tree_delete(struct TreeNode** leaf);
