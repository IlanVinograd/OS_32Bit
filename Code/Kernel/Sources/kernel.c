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
    //test_alloc_medium();
    test_alloc_hard();
    //test_alloc_even_hard();

    while (1) __asm__ ("hlt");  // Loop indefinitely
}
