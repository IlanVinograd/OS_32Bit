#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <stdint.h>
#include <string.h>
#include <paging.h>
#include <kernel.h>

#define MEM_ALLOC_START       0x00400000
#define MEM_ALLOC_END         0x007FFFFF
#define MEM_ALLOC_SIZE        (MEM_ALLOC_END - MEM_ALLOC_START)
#define PAGE_SIZE             4096
#define NUM_PAGES             ((MEM_ALLOC_END - MEM_ALLOC_START) / PAGE_SIZE) // 0 to 1022 = 1023 Pages.
#define BITMAP_SIZE           ((NUM_PAGES + 31) / 32) // Bitmap size in uint32_t (32 bits per uint32_t)

extern uint32_t bitmap[BITMAP_SIZE];
extern uint32_t pagesAllocated;

void print_bitmap();
void  init_bitmap();
void* find_free_pages(uint32_t num_pages);
void* find_allocated_page();
bool_t is_page_free(uint32_t page_number);
void  mark_pages_as_used(uint32_t page_number);
void mark_page_as_free(uint32_t page_number);

#endif