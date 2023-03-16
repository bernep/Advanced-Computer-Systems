/* bin_tree.h */

// Import libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definitions
#define MAXLEN 16 // maximum string length
typedef unsigned int uint;  // for convenience
typedef int (*TreeCompare)(const char*, const char*); // for convenience

// B-tree Node struct
struct TreeNode {
    char* value;            // all void* types replaced by char*
    uint encoded_value;
    struct TreeNode* p_left;
    struct TreeNode* p_right;
};

// Function prototypes
int tree_insert(char* key, uint encoded_val, struct TreeNode** leaf, TreeCompare cmp);
int tree_cmp_str(const char* a, const char* b);
void tree_print(struct TreeNode* root);
uint tree_search(char* key, struct TreeNode* leaf, TreeCompare cmp);
void tree_delete(struct TreeNode** leaf);
char* tree_prefix_lookup(struct TreeNode* node, const char* value);