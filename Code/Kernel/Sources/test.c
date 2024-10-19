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