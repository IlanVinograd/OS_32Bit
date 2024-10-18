#include "../Includes/bitmap.h"

uint32_t bitmap[BITMAP_SIZE];

void init_bitmap(){
    memset(bitmap, 0, sizeof(bitmap));
    printf("BITMAP_SIZE: %d\n",COLOR_BLACK_ON_WHITE, BITMAP_SIZE);
}

void* find_free_pages(uint32_t num_pages){
    return NULL;
}

void  mark_pages_as_used(void* allocated_address, uint32_t num_pages){
    return NULL;
}