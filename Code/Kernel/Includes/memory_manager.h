#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

#include <stdint.h>
#include <bitmap.h>
#include <kernel.h>

void* page_alloc(uint32_t num_pages);
void* malloc(uint32_t size);
void free_large_pages(void* block, uint32_t size);
void free(void* ptr);

#endif