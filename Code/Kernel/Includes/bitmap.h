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
#define NUM_PAGES             (MEM_ALLOC_SIZE / PAGE_SIZE)
#define BITMAP_SIZE           ((NUM_PAGES + 31) / 32)  // Bitmap size in uint32_t (32 bits per uint32_t)

extern uint32_t bitmap[BITMAP_SIZE];

void  init_bitmap();
void* find_free_pages(uint32_t num_pages);
void  mark_pages_as_used(void* allocated_address, uint32_t num_pages);

#endif