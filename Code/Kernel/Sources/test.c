#include "../Includes/test.h"
extern TextStyle backGroundColor;
extern uint16_t keyboard_cursor_position;

void delay(uint32_t count) {
    while (count--) {
        __asm__ volatile ("nop");
    }
}

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

    test_full_allocation();
    test_fragmentation();
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

volatile int count = 0;

void task1_entry() {
    setCursorPosition(0, 0);
     __asm__ ("cli"); // Disable interrupts before critical section
    printf("Task 1 Start - %d\n", backGroundColor, count);
     __asm__ ("sti"); // Re-enable interrupts after critical section

    for (int i = 1; i <= 100000; i++) {
         __asm__ ("cli"); // Disable interrupts before modifying shared resource
        count++;
        setCursorPosition(10, 0);
        printf("Loop count 1 -> %d", backGroundColor, count);
         __asm__ ("sti"); // Re-enable interrupts after modification
    }

     __asm__ ("cli"); // Disable interrupts before critical section
    setCursorPosition(4, 0);
    printf("Task 1 Finished - %d\n", backGroundColor, count);
     __asm__ ("sti"); // Re-enable interrupts after critical section

    set_task_state(current, TERMINATED); // Mark task as terminated.
    yield(); // Yield control explicitly.
}

void task2_entry() {
    setCursorPosition(1, 0);
     __asm__ ("cli"); // Disable interrupts before critical section
    printf("Task 2 Start - %d\n", backGroundColor, count);
     __asm__ ("sti"); // Re-enable interrupts after critical section

    for (int i = 1; i <= 100000; i++) {
         __asm__ ("cli"); // Disable interrupts before modifying shared resource
        count++;
        setCursorPosition(11, 0);
        printf("Loop count 2 -> %d", backGroundColor, count);
         __asm__ ("sti"); // Re-enable interrupts after modification

        if (i % 1000 == 0) {
            yield(); // Yield control to another task.
        }
    }

     __asm__ ("cli"); // Disable interrupts before critical section
    setCursorPosition(5, 0);
    printf("Task 2 Finished - %d\n", backGroundColor, count);
     __asm__ ("sti"); // Re-enable interrupts after critical section

    set_task_state(current, TERMINATED); // Mark task as terminated.
    yield(); // Yield control explicitly.
}

void test() {
    unlock_scheduler(); // Ensure the scheduler is unlocked.

    clearScreen();

    // Create tasks and add them to the scheduler.
    create_task((uintptr_t)task1_entry);
    create_task((uintptr_t)task2_entry);

    // Set current task state to TERMINATED to indicate it is finished.
    set_task_state(current, TERMINATED);
    yield(); // Hand over control to the scheduler.
}

void test_full_allocation() {
    printf("Testing full memory allocation...\n", GREEN_ON_BLACK_SUCCESS);
    // Allocate all memory
    while (malloc(1024 * 1024)) {

    }
    printf("Full memory allocation test completed.\n", GREEN_ON_BLACK_SUCCESS);
}

void test_fragmentation() {
    uint16_t row = keyboard_cursor_position / VGA_COLS;

    scrollIfNeeded(row);
    printf("Testing memory fragmentation...\n", GREEN_ON_BLACK_SUCCESS);
    ++row;
    scrollIfNeeded(row);

    void *ptrs[2];
    for (int i = 0; i < 2; i++) {
        ptrs[i] = malloc(512 * 1024);
    }
    for (int i = 0; i < 2; i++) {
        free(ptrs[i]);
    }
    for (int i = 0; i < 2; i++) {
        ptrs[i] = malloc(256 * 1024);
    }

    scrollIfNeeded(row);
    printf("Memory fragmentation test completed.\n", GREEN_ON_BLACK_SUCCESS);
    ++row;
    scrollIfNeeded(row);

    setCursorPosition(row, 0);
    keyboard_cursor_position = row * VGA_COLS;
}