#include "../Includes/binary_tree.h"

binary_tree_node_t* root = NULL;  // Initialize root for the binary tree

// Function to find a free block in the binary tree
void* find_free_block_in_binary_tree(binary_tree_node_t* node, uint32_t size) {
    if (node == NULL) {
        return NULL;
    }
    
    // Traverse the binary tree to find a free block of the requested size
    if (node->is_free && node->size >= size) {
        node->is_free = 0;  // Mark the block as used
        return (void*)((uintptr_t)node->address + KERNEL_HIGH_BASE);  // Adjust address with KERNEL_HIGH_BASE
    }
    
    // Traverse the tree (left for smaller, right for larger)
    if (size < node->size / 2) {
        return find_free_block_in_binary_tree(node->left, size);
    } else {
        return find_free_block_in_binary_tree(node->right, size);
    }
}

// Function to initialize a binary tree for a new page
void init_binary_tree_for_page(void* page_start) {
    // Allocate a page for the binary tree node
    root = (binary_tree_node_t*)page_alloc(1);

    if (root == NULL) {
        // Handle allocation failure
        return;
    }

    root->size = PAGE_SIZE;
    root->address = page_start;  // Keep the physical address
    root->left = root->right = NULL;
    root->is_free = 1;  // Initially, the whole page is free
}


// Function to allocate a block from the page's binary tree
void* allocate_block_from_page(binary_tree_node_t* node, uint32_t size) {
    if (node == NULL || !node->is_free || node->size < size) {
        return NULL;
    }

    if (node->size == size) {
        node->is_free = 0;  // Exact match, mark as used
        return (void*)((uintptr_t)node->address + KERNEL_HIGH_BASE);  // Adjust address with KERNEL_HIGH_BASE
    }

    // If the block is too large, split it into two
    if (node->left == NULL && node->right == NULL) {
        node->left = (binary_tree_node_t*)page_alloc(1);
        node->right = (binary_tree_node_t*)page_alloc(1);

        if (node->left == NULL || node->right == NULL) {
            // Handle allocation failure
            return NULL;
        }

        node->left->size = node->size / 2;
        node->left->address = node->address;
        node->left->is_free = 1;
        node->left->left = node->left->right = NULL;

        node->right->size = node->size / 2;
        node->right->address = (void*)((uintptr_t)node->address + node->size / 2);
        node->right->is_free = 1;
        node->right->left = node->right->right = NULL;
    }

    // Allocate from the left child if the requested size is smaller
    if (size <= node->left->size) {
        return allocate_block_from_page(node->left, size);
    } else {
        return allocate_block_from_page(node->right, size);
    }
}