#include "../Includes/kernel.h"

void _start(void) {
    // Remove the identitiy mapping to the first 4MiB
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
    You can uncomment the following ISR code and test it yourself.
    */

    /*
    // Trigger Divide by Zero Exception (ISR0)
    // Creating C code that causes a division by 0 is undefined behaviour.
    // Use inline assembly to produce a divide by zero.
    __asm__ ("div %b0" :: "a"(0));
    */

    /*
    // Trigger Invalid Opcode Exception (ISR6)
    __asm__("ud2");
    */

    /*
    // Test General Protection Fault (ISR13)
    __asm__("int $13");  // This will trigger ISR13
    */

    // Mark volatile so the optimizer can't optimize away ptr altogether
    
    // Test Page Fault (ISR14) - This will only work if paging is enabled
    /*
    volatile uint8_t *ptr = (uint8_t *)0xC0900000;  // A range that might not be mapped
    uint8_t value = *ptr;  // This should trigger a page fault if it's unmapped
    (void)value;  // Avoid compiler warning
    */

    
    

    while (1) __asm__ ("hlt");  // Loop indefinitely
}