/* bin_tree.h */

// Import libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <smmintrin.h> // SSE4.1

// Definitions
#define MAX_STR_LEN 100  // maximum string length
typedef unsigned int uint;  // for convenience
typedef int (*TreeCompare)(const char*, const char*);  // for convenience

// Binary tree Node struct
struct TreeNode {
    char* value;
    uint encoded_value;
    struct ListNode* indices;
    struct TreeNode* p_left;
    struct TreeNode* p_right;
};

// Linked list struct (for entry indexing)
struct ListNode {
   int index;
   struct ListNode* next;
};

// Function prototypes
uint tree_insert(char* key, uint index, uint encoded_val, struct TreeNode** leaf);
void tree_print(struct TreeNode* root);
uint tree_str_search(char* key, struct TreeNode* leaf);
void tree_code_search(uint code, char* str_res, struct TreeNode* leaf);
void tree_prefix_search(const char* prefix, uint* codes,
                        size_t* codes_size, struct TreeNode* leaf);
int simd_strcmp(const char* str1, const char* str2);
uint tree_simd_str_search(char* key, struct TreeNode* leaf);
int simd_prefix_strcmp(const char *a, const char *b);
void tree_simd_prefix_search(const char* prefix, uint* codes,
                        size_t* codes_size, struct TreeNode* leaf);
struct ListNode* tree_get_indices(uint code, struct TreeNode* leaf);
void tree_delete(struct TreeNode** leaf);
