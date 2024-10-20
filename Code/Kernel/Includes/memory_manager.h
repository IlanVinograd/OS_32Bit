#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

#include <stdint.h>
#include <bitmap.h>

void* page_alloc(uint32_t num_pages);
void* malloc(uint32_t size);
void free(void* ptr);
void* find_free_block(uint32_t size);
void mark_block_as_used(void* ptr, uint32_t size);
void mark_block_as_free(void* ptr, uint32_t size);

#endif