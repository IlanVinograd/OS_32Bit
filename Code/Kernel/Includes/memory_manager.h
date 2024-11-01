#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

#include <stdint.h>
#include <bitmap.h>
#include <kernel.h>

#define PAGES_FOR_HEAP 4
typedef struct FreeBlock {
    size_t size;
    void* address;
    struct FreeBlock* next;
    bool_t isFree;
} FreeBlock;

typedef struct AllocationHeader {
    uint32_t num_pages;
} AllocationHeader;

void* page_alloc(uint32_t num_pages);
void free_large_pages(void* block, uint32_t size);
void init_free_list();
void* malloc(uint32_t size);
void free(void* block);


#endif