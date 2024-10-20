#include "../Includes/memory_manager.h"

void* page_alloc(uint32_t num_pages) {
    void* allocated_address = find_free_pages(num_pages);

    if (allocated_address == NULL) {
        printf("Warning: page_alloc failed to allocate memory! [Page Allocation Error Code: %d]\n", RED_ON_BLACK_WARNING, allocated_address);
        return NULL;
    }

    // Adjust the allocated address with the KERNEL_HIGH_BASE
    return (void*)((uintptr_t)allocated_address + KERNEL_HIGH_BASE);
}

void* malloc(uint32_t size) {
    // Handle small allocations using buddy system
    if (size > 0 && size <= PAGE_SIZE) {
        // Check if there is an available binary tree for an existing page
        void* allocated_block = find_free_block_in_binary_tree(root, size);
        
        if (allocated_block != NULL) {
            // If a suitable block was found, return it
            return allocated_block;
        } else {
            // No suitable block found, allocate a new page
            void* new_page = page_alloc(1);
            
            if (new_page == NULL) {
                // If page allocation failed, return NULL
                return NULL;
            }

            // Initialize a binary tree for the new page and allocate from it
            init_binary_tree_for_page(new_page);
            return allocate_block_from_page(root, size);
        }
    } 
    // Handle large allocations (size > PAGE_SIZE)
    else if (size > PAGE_SIZE) {
        // Calculate how many pages are needed
        uint32_t num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
        
        // Allocate the necessary pages
        void* large_alloc = page_alloc(num_pages);
        
        if (large_alloc == NULL) {
            // If page allocation failed, return NULL
            return NULL;
        }

        return large_alloc;
    }

    return NULL;  // Invalid size request
}

// Helper function to mark pages as free in the bitmap
void free_large_pages(void* block, uint32_t size) {
    uintptr_t address = (uintptr_t)block - KERNEL_HIGH_BASE;  // Adjust to physical address
    uint32_t start_page = (address - MEM_ALLOC_START) / PAGE_SIZE;
    uint32_t num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;  // Calculate the number of pages

    // Mark each page as free in the bitmap
    for (uint32_t i = 0; i < num_pages; i++) {
        mark_page_as_free(start_page + i);
    }
}

void free(void* block) {
    // Check if the block is within the page allocation range
    if ((uintptr_t)block >= KERNEL_HIGH_BASE + MEM_ALLOC_START && (uintptr_t)block <= KERNEL_HIGH_BASE + MEM_ALLOC_END) {
        // We are dealing with regular page-based allocations
        uintptr_t block_address = (uintptr_t)block - KERNEL_HIGH_BASE;  // Get physical address

        // Determine if this is a large allocation (more than one page)
        if ((uintptr_t)block_address + PAGE_SIZE <= MEM_ALLOC_END) {
            // This is a large block (page-allocated memory), so we free the pages
            free_large_pages(block, PAGE_SIZE);
            return;
        }
    }

    // Otherwise, it's a block allocated through the binary tree for < 4096 bytes
    binary_tree_node_t* node = root;

    while (node != NULL) {
        void* real_address = (void*)((uintptr_t)node->address + KERNEL_HIGH_BASE);
        
        if (real_address == block) {
            node->is_free = 1;
            break;
        }
        
        if ((uintptr_t)block < (uintptr_t)real_address + node->size / 2) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
}