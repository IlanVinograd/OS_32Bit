#ifndef _BINARY_TREE_H_
#define _BINARY_TREE_H_

#include <stdint.h>
#include <bitmap.h>
#include <kernel.h>

typedef struct binary_tree_node {
    uint32_t size;  // Size of the block
    void* address;  // Pointer to the start of the block
    struct binary_tree_node* left;  // Left child (smaller block)
    struct binary_tree_node* right; // Right child (larger block)
    int is_free;    // Whether the block is free
} binary_tree_node_t;

// Externally accessible functions
void* find_free_block_in_binary_tree(binary_tree_node_t* root, uint32_t size);
void init_binary_tree_for_page(void* page_start);
void* allocate_block_from_page(binary_tree_node_t* root, uint32_t size);

// Externally accessible global variables
extern binary_tree_node_t* root;

#endif