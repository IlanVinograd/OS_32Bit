#include "../Includes/test.h"

void test_bitmap() {
    // Edge Case 1: Allocate the first page
    printf("Test 1: Allocating the first page (0)...\n", COLOR_BLACK_ON_WHITE);
    void* addr1 = find_free_pages(1);
    if (addr1 != NULL && addr1 == (void*)MEM_ALLOC_START) {
        printf("Success: First page allocated at address %p\n", GREEN_ON_BLACK_SUCCESS, addr1);
    } else {
        printf("Error: Failed to allocate the first page\n", RED_ON_BLACK_WARNING);
    }
    
    // Edge Case 2: Allocate the last page explicitly by ensuring it's the only free page
    printf("Test 2: Allocating the last page (%u)...\n", COLOR_BLACK_ON_WHITE, NUM_PAGES);

    // Ensure all pages are marked as used except the last one
    init_bitmap();  // Reset the bitmap first
    for (uint32_t i = 0; i < NUM_PAGES - 1; i++) {
        mark_pages_as_used(i);  // Mark all pages except the last one as used
    }

    mark_page_as_free(NUM_PAGES - 1);  // Ensure the last page (1023) is free

    // Now allocate the last page
    void* addr2 = find_free_pages(1);

    if (addr2 != NULL && addr2 == (void*)(MEM_ALLOC_START + (NUM_PAGES - 1) * PAGE_SIZE)) {
        printf("Success: Last page allocated at address %p\n", GREEN_ON_BLACK_SUCCESS, addr2);
        *(uint8_t*)((uintptr_t)addr2 + KERNEL_HIGH_BASE + 4095) = 1; // You can check with GDB this address to verify value (0x1).
    } else {
        printf("Error: Failed to allocate the last page\n", RED_ON_BLACK_WARNING);
    }
    
    // Edge Case 3: Allocate all available memory
    printf("Test 3: Allocating all available pages...\n", COLOR_BLACK_ON_WHITE);
    init_bitmap();  // Reset bitmap
    void* addr3 = find_free_pages(NUM_PAGES);
    if (addr3 != NULL) {
        printf("Success: All pages allocated starting at address %p\n", GREEN_ON_BLACK_SUCCESS, addr3);
    } else {
        printf("Error: Failed to allocate all pages\n", RED_ON_BLACK_WARNING);
    }
    
    // Edge Case 4: Allocate more pages than available
    printf("Test 4: Allocating more pages than available (%u + 1)...\n", COLOR_BLACK_ON_WHITE, NUM_PAGES);
    void* addr4 = find_free_pages(NUM_PAGES + 1);
    if (addr4 == NULL) {
        printf("Success: Correctly handled request for more pages than available\n", GREEN_ON_BLACK_SUCCESS);
    } else {
        printf("Error: Unexpectedly allocated pages\n", RED_ON_BLACK_WARNING);
    }
    
    // Edge Case 5: Allocate exactly 32 pages (boundary test)
    printf("Test 5: Allocating exactly 32 pages (crossing bitmap boundary)...\n", COLOR_BLACK_ON_WHITE);
    init_bitmap();
    void* addr5 = find_free_pages(1023);  // Allocating 1023 pages
    if (addr5 != NULL) {
        printf("Success: 32 pages allocated at address %p\n", GREEN_ON_BLACK_SUCCESS, addr5);
    } else {
        printf("Error: Failed to allocate 32 pages\n", RED_ON_BLACK_WARNING);
    }
    
    // Edge Case 6: Allocate and free scattered pages, then allocate a block
    printf("Test 6: Fragmentation test...\n", COLOR_BLACK_ON_WHITE);
    init_bitmap();  // Reset bitmap

    // Allocate scattered pages
    for (uint32_t i = 0; i < 10; i++) {
        find_free_pages(1);  // Allocate 10 individual pages
    }

    // Free 5 scattered pages
    for (uint32_t i = 0; i < 10; i += 2) {
        mark_page_as_free(i);
    }

    // Now try to allocate a block of 3 pages
    void* addr6 = find_free_pages(3);
    if (addr6 != NULL) {
        printf("Success: Allocated block of 3 pages after freeing scattered pages\n", GREEN_ON_BLACK_SUCCESS);
    } else {
        printf("Error: Failed to allocate block of 3 pages\n", RED_ON_BLACK_WARNING);
    }
}

void test_isr0(){
    // Trigger Divide by Zero Exception (ISR0)
    // Creating C code that causes a division by 0 is undefined behaviour.
    // Use inline assembly to produce a divide by zero.
    __asm__ ("div %b0" :: "a"(0));
}

void test_isr6(){
    // Trigger Invalid Opcode Exception (ISR6)
    __asm__("ud2");
}

void test_isr13(){
    // Test General Protection Fault (ISR13)
    __asm__("int $13");  // This will trigger ISR13
}
    
void test_isr14(){
    // Test Page Fault (ISR14) - This will only work if paging is enabled
    volatile uint8_t *ptr = (uint8_t *)0xC0900000;  // A range that might not be mapped
    uint8_t value = *ptr;  // This should trigger a page fault if it's unmapped
    (void)value;  // Avoid compiler warning
}

void test_alloc_medium(){
    // Slightly harder tests //
int i = 0;

// Loop with more iterations and slightly larger allocations
while (i < 40) {  // Increased the number of iterations slightly
    setCursorPosition(0, 0);
    
    // Allocate small block
    void* ptr1 = malloc(75);  // Slightly larger allocation
    setCursorPosition(0, 0);
    print_bitmap();

    // Allocate small block well below page size
    void* ptr2 = malloc(1200);  // Increased size but still small
    setCursorPosition(0, 0);
    print_bitmap();

    // Free and reallocate memory in different order
    free(ptr1);
    void* ptr3 = malloc(800);  // Moderate reallocation
    setCursorPosition(0, 0);
    print_bitmap();

    // Free small block
    free(ptr2);
    void* ptr4 = malloc(1200);  // Similar size as ptr2
    setCursorPosition(0, 0);
    print_bitmap();

    // Free everything
    free(ptr3);
    free(ptr4);

    i++;
    }

// Boundary testing with slightly more iterations and larger blocks
for (int j = 0; j < 20; j++) {  // Increased the iterations a bit
    void* ptr1 = malloc(1024);  // One-quarter of a page
    setCursorPosition(0, 0);
    print_bitmap();
    
    void* ptr2 = malloc(50);  // Small allocation
    setCursorPosition(0, 0);
    print_bitmap();

    void* ptr3 = malloc(512);  // Moderate block size
    setCursorPosition(0, 0);
    print_bitmap();

    // Free them in specific order
    free(ptr2);
    free(ptr1);
    free(ptr3);

    void* ptr4 = malloc(1800); // Moderate block, below half a page
    setCursorPosition(0, 0);
    print_bitmap();

    free(ptr4);
    }

// Interleaved malloc/free with small and moderate sizes
for (int k = 0; k < 35; k++) {  // Increased complexity slightly
    void* ptr1 = malloc(150);    // Small allocation
    void* ptr2 = malloc(400);    // Moderate size
    void* ptr3 = malloc(1000);   // Larger but still below a page
    setCursorPosition(0, 0);
    print_bitmap();

    free(ptr2);  // Free in the middle
    void* ptr4 = malloc(600);   // Slightly larger reallocation
    setCursorPosition(0, 0);
    print_bitmap();

    free(ptr1);
    free(ptr3);
    free(ptr4);
    setCursorPosition(0, 0);
    print_bitmap();
    }   
}

void test_alloc_hard(){
    // Harder tests //
int i = 0;

// Loop with more iterations and larger allocations
while (i < 50) {  // Increased the number of iterations slightly more
    setCursorPosition(0, 0);
    
    // Allocate small block
    void* ptr1 = malloc(100);  // Slightly larger allocation
    setCursorPosition(0, 0);
    print_bitmap();

    // Allocate small block well below page size
    void* ptr2 = malloc(1500);  // Increased size
    setCursorPosition(0, 0);
    print_bitmap();

    // Free and reallocate memory in different order
    free(ptr1);
    void* ptr3 = malloc(1000);  // Larger reallocation
    setCursorPosition(0, 0);
    print_bitmap();

    // Free small block
    free(ptr2);
    void* ptr4 = malloc(2000);  // Larger block size
    setCursorPosition(0, 0);
    print_bitmap();

    // Free everything
    free(ptr3);
    free(ptr4);

    i++;
    }

// Boundary testing with larger blocks and moderate iterations
for (int j = 0; j < 30; j++) {  // Increased the iterations and size
    void* ptr1 = malloc(1500);  // Moderate allocation size
    setCursorPosition(0, 0);
    print_bitmap();
    
    void* ptr2 = malloc(75);  // Small allocation
    setCursorPosition(0, 0);
    print_bitmap();

    void* ptr3 = malloc(800);  // Moderate block size
    setCursorPosition(0, 0);
    print_bitmap();

    // Free them in specific order
    free(ptr2);
    free(ptr1);
    free(ptr3);

    void* ptr4 = malloc(2500); // Slightly larger block
    setCursorPosition(0, 0);
    print_bitmap();

    free(ptr4);
    }

// Interleaved malloc/free with larger sizes
for (int k = 0; k < 40; k++) {  // Slightly increased iterations
    void* ptr1 = malloc(200);    // Small allocation
    void* ptr2 = malloc(600);    // Moderate size
    void* ptr3 = malloc(1500);   // Larger block below full page
    setCursorPosition(0, 0);
    print_bitmap();

    free(ptr2);  // Free in the middle
    void* ptr4 = malloc(1000);   // Larger reallocation
    setCursorPosition(0, 0);
    print_bitmap();

    free(ptr1);
    free(ptr3);
    free(ptr4);
    setCursorPosition(0, 0);
    print_bitmap();
    }
}

void test_alloc_even_hard(){
    // Harder tests //
int i = 0;

// Loop with increased iterations and larger allocations
while (i < 70) {  // Increased the number of iterations
    setCursorPosition(0, 0);
    
    // Allocate moderate block
    void* ptr1 = malloc(200);  // Slightly larger allocation
    setCursorPosition(0, 0);
    print_bitmap();

    // Allocate larger block well below page size
    void* ptr2 = malloc(2200);  // Increased size
    setCursorPosition(0, 0);
    print_bitmap();

    // Free and reallocate memory in different order
    free(ptr1);
    void* ptr3 = malloc(1500);  // Larger reallocation
    setCursorPosition(0, 0);
    print_bitmap();

    // Free larger block
    free(ptr2);
    void* ptr4 = malloc(3000);  // Large block size
    setCursorPosition(0, 0);
    print_bitmap();

    // Free everything
    free(ptr3);
    free(ptr4);

    i++;
    }

// Boundary testing with larger blocks and more iterations
for (int j = 0; j < 50; j++) {  // Increased the iterations more
    void* ptr1 = malloc(2500);  // Larger allocation size
    setCursorPosition(0, 0);
    print_bitmap();
    
    void* ptr2 = malloc(150);  // Small allocation
    setCursorPosition(0, 0);
    print_bitmap();

    void* ptr3 = malloc(1200);  // Moderate block size
    setCursorPosition(0, 0);
    print_bitmap();

    // Free them in specific order
    free(ptr2);
    free(ptr1);
    free(ptr3);

    void* ptr4 = malloc(3500); // Larger block
    setCursorPosition(0, 0);
    print_bitmap();

    free(ptr4);
    }

// Interleaved malloc/free with larger sizes
for (int k = 0; k < 60; k++) {  // Further increased iterations
    void* ptr1 = malloc(300);    // Small allocation
    void* ptr2 = malloc(1000);   // Moderate size
    void* ptr3 = malloc(2200);   // Larger block, still below a page
    setCursorPosition(0, 0);
    print_bitmap();

    free(ptr2);  // Free in the middle
    void* ptr4 = malloc(1600);   // Larger reallocation
    setCursorPosition(0, 0);
    print_bitmap();

    free(ptr1);
    free(ptr3);
    free(ptr4);
    setCursorPosition(0, 0);
    print_bitmap();
    }
}