#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

#include <stdint.h>
#include <bitmap.h>

void* page_alloc(uint32_t num_pages);
void* malloc(uint32_t size);

#endif