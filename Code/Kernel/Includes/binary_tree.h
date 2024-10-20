#ifndef _BINARY_TREE_H_
#define _BINARY_TREE_H_

#include <stdint.h>

void init_binary_tree(); 
void* find_suitable_block(uint32_t size);
void split_block(void* block, uint32_t original_size, uint32_t new_size);
void merge_buddy_blocks(void* block1, void* block2);
bool_t is_buddy(void* block1, void* block2);

#endif