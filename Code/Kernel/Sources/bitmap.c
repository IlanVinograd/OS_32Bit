#include "../Includes/bitmap.h"

uint32_t bitmap[BITMAP_SIZE];

void print_bitmap(){
    printf("Bitmap state:\n", COLOR_BLACK_ON_WHITE);

    for(int page_number = 0; page_number < NUM_PAGES; page_number++){
        uint32_t index = page_number / 32;
        uint32_t bit = page_number % 32;

        // Check if the page is used or free and print its state
        if (bitmap[index] & (1 << bit)) {
            printf("1", COLOR_BLACK_ON_WHITE); // Page is used
        } else {
            printf("0", COLOR_BLACK_ON_WHITE); // Page is free
        }

        // Add a new line every 32 pages to make it more readable
        if ((page_number + 1) % 50 == 0) {
            printf("\n", COLOR_BLACK_ON_WHITE);
        }
    }
}

void init_bitmap(){
    memset(bitmap, 0, sizeof(bitmap));
}

void* find_allocated_page() {
    for (uint32_t page_number = 0; page_number < NUM_PAGES; page_number++) {
        void* allocated_address;
        if(!is_page_free(page_number)){
            allocated_address = (void*)(MEM_ALLOC_START + (page_number * PAGE_SIZE));
            return allocated_address;
        }
    }

    // If no block of allocated page was found, return NULL
    printf("Error: No block of allocated page found!\n", RED_ON_BLACK_WARNING);
    return NULL;
}

void* find_free_pages(uint32_t num_pages) {
    if(num_pages > 1023) {
        printf("Error: Page number out of bounds!\n", RED_ON_BLACK_WARNING);
        return NULL;
    }

    uint32_t consecutive_free_pages = 0;
    uint32_t start_page = NUM_PAGES;  // Initialize to an invalid page number NUM_PAGES = 31, 0-31.

    // Iterate over the bitmap to find consecutive free pages
    for (uint32_t page_number = 0; page_number < NUM_PAGES; page_number++) {
        if (is_page_free(page_number)) {
            if (consecutive_free_pages == 0) {
                start_page = page_number;
            }
            consecutive_free_pages++;

            if (consecutive_free_pages == num_pages) {
                // Mark the pages as used
                for (uint32_t i = 0; i < num_pages; i++) {
                    mark_pages_as_used(start_page + i);
                }

                // Calculate the allocated address
                void* allocated_address = (void*)(MEM_ALLOC_START + (start_page * PAGE_SIZE));
                return allocated_address;
            }
        } else {
            consecutive_free_pages = 0;  // Reset if a used page is found
        }
    }

    // If no block of free pages was found, return NULL
    printf("Error: No block of %u consecutive free pages found!\n", RED_ON_BLACK_WARNING, num_pages);
    return NULL;
}

bool_t is_page_free(uint32_t page_number) {
    if (page_number > NUM_PAGES) {
        printf("Error: Page number out of bounds!\n", RED_ON_BLACK_WARNING);
        return 0;  // Return false instead of NULL
    }

    uint32_t index = page_number / 32;
    uint32_t bit = page_number % 32;

    return !(bitmap[index] & (1 << bit));  // Return 1 if free (bit is 0)
}

void mark_pages_as_used(uint32_t page_number) {
    if (page_number > NUM_PAGES) {
        printf("Error: Page number out of bounds!\n", RED_ON_BLACK_WARNING);
        return;
    }

    uint32_t index = page_number / 32;
    uint32_t bit = page_number % 32;

    bitmap[index] |= (1 << bit);  // Set the bit to 1 (mark as used)
}

void mark_page_as_free(uint32_t page_number) {
    if (page_number > NUM_PAGES) {
        printf("Error: Page number out of bounds!\n", RED_ON_BLACK_WARNING);
        return;
    }

    uint32_t index = page_number / 32;
    uint32_t bit = page_number % 32;

    bitmap[index] &= ~(1 << bit);  // Set the bit to 0 (mark as free)
}