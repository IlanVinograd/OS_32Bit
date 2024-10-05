#include "../Includes/idt.h"

// Declaring IDT array and pointer
struct idt_entry idt[256];
struct idt_ptr idt_p;

// These are external assembly functions, declared but not defined here
extern void isr0();
extern void load_idt(void);

// Set the IDT gate with the given interrupt handler address
void set_idt_gate(int n, uint32_t handler) {
    idt[n].base_low = handler & 0xFFFF;
    idt[n].sel = 0x08;  // Kernel code segment selector
    idt[n].always0 = 0;
    idt[n].flags = 0x8E;  // Interrupt gate, kernel mode
    idt[n].base_high = (handler >> 16) & 0xFFFF;
}

// Initialize the IDT
void init_idt(void) {
    idt_p.limit = sizeof(idt) - 1;
    idt_p.base = (uint32_t)&idt;

    // Set IDT gates (add more interrupt handlers here as needed)
    set_idt_gate(0, (uint32_t)isr0);  // Set interrupt 0 (divide by zero exception)

    // Load the IDT using the assembly function
    load_idt();
}

void isr0_handler(void) {
    printf("Divide by Zero Exception!\n", RED_ON_BLACK_WARNING);
}