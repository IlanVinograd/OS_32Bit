#include "../Includes/memory_manager.h"

void* page_alloc(uint32_t num_pages){
    void* allocated_address = find_free_pages(num_pages);

    if (allocated_address == NULL) {
        printf("Warning: page_alloc failed to allocate memory! [Page Allocation Error Code: %d]\n", RED_ON_BLACK_WARNING, allocated_address);
        return NULL;
    }
    return allocated_address;
}

void* malloc(uint32_t size){
    return NULL;
}