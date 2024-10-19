#include "../Includes/memory_manager.h"

void* page_alloc(uint32_t num_pages){
    void* allocated_address = find_free_pages(num_pages);

    if (allocated_address == NULL) return NULL;

    //mark_pages_as_used(page_number);

    return allocated_address;
}

void* malloc(uint32_t size){
    return NULL;
}