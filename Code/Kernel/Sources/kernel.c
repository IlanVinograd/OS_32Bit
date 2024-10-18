#include "../Includes/kernel.h"

void _start(void) {
    // Remove the identitiy mapping to the first 4MiB
    remove_first_4MiB_mapping();

    init_idt();
    init_gdt();

    init_segments();

    clearScreen();
    setCursorPosition(0, 0);
    //init_memory_map();
    //initScreen("0.3");

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
    volatile uint32_t *ptr = (uint32_t*)0xC0400000;  // Invalid address
    uint32_t data = *ptr;  // Dereference invalid memory (triggers ISR14)
    (void) data;           // Avoid compiler warning about unused variable
    

    while (1) __asm__ ("hlt");  // Loop indefinitely
}
