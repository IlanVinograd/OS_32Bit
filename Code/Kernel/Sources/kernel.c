#include "../Includes/kernel.h"

void _start(void) {
    init_idt();  // Initialize the IDT
    init_gdt();

    init_segments();

    clearScreen();
    setCursorPosition(0, 0);
    initScreen("0.3");

    /*
    You can uncomment the following ISR code and test it yourself.
    */

    /*
    // Trigger Divide by Zero Exception (ISR0)
    int a = 10;
    int b = 0;
    int c = a / b;
    */

    /*
    // Trigger Invalid Opcode Exception (ISR6)
    asm volatile ("ud2");
    */

    /*
    // Test General Protection Fault (ISR13)
    asm volatile ("int $13");  // This will trigger ISR13
    */

    /*
    // Test Page Fault (ISR14) - This will only work if paging is enabled
    uint32_t *ptr = (uint32_t*)0xFFFFFFFF;  // Invalid address
    uint32_t data = *ptr;  // Dereference invalid memory (triggers ISR14)
    */

    while (1) __asm__ __volatile__ ("hlt");  // Loop indefinitely
}
