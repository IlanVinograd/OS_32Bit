#include "../Includes/kernel.h"

void _start(void) {
    // Remove the identity mapping to the first 4MiB
    remove_first_4MiB_mapping();

    init_idt();
    init_gdt();
    init_segments();

    clearScreen();
    init_bitmap();
    setCursorPosition(0, 0);
    //initScreen("0.3");
    //init_memory_map();
    /*
    page_alloc(213);
    mark_pages_as_used(214);
    mark_pages_as_used(216);
    mark_pages_as_used(218);
    mark_pages_as_used(220);
    page_alloc(5);
    mark_page_as_free(0);
    printf("%p", COLOR_BLINKING_YELLOW, find_allocated_page());
    */
    // tests //
    //test_bitmap();

    init_free_list();
    //print_bitmap();
    //test_malloc_fragmentation_large_scale();
    
    void* ptr1 = malloc(3);
    *(unsigned char*)ptr1 = 15;
    void* ptr2 = malloc(3);
    *(unsigned char*)ptr2 = 1;
    void* ptr3 = malloc(2);
    *(unsigned char*)ptr3 = 15;
    //free(ptr2);
    void* ptr4 = malloc(1);
    *(unsigned char*)ptr4 = 255;

    free(ptr2);
    void* ptr5 = malloc(2);
    *(unsigned char*)ptr5 = 100;

    void* ptr6 = malloc(1);
    *(unsigned char*)ptr6 = 15;

    ptr5 = realloc(ptr5, 1);
    printf("%p | %d\n", COLOR_BLINKING_YELLOW, ptr1, *(unsigned char*)ptr1);
    printf("%p | %d\n", COLOR_BLINKING_YELLOW, ptr2, *(unsigned char*)ptr2);
    printf("%p | %d\n", COLOR_BLINKING_YELLOW, ptr3, *(unsigned char*)ptr3);
    printf("%p | %d\n", COLOR_BLINKING_YELLOW, ptr4, *(unsigned char*)ptr4);
    printf("%p | %d\n", COLOR_BLINKING_YELLOW, ptr5, *(unsigned char*)ptr5);
    printf("%p | %d\n", COLOR_BLINKING_YELLOW, ptr6, *(unsigned char*)ptr6);
    
    while (1) __asm__ ("hlt");  // Loop indefinitely
}
