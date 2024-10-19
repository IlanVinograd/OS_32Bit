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

    // tests //
    test_bitmap();

    while (1) __asm__ ("hlt");  // Loop indefinitely
}
