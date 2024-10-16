#include "../Includes/idt.h"

// Declaring IDT array
struct idt_entry idt[256];

// These are external assembly functions, declared but not defined here
extern void isr0();
extern void isr6();
extern void isr13();
extern void isr14();

void load_idt(struct idt_ptr* idtp) {
    __asm__ ("lidt %0" :: "m"(*idtp) : "memory");
}

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
    // Declaring IDT pointer
    struct idt_ptr idt_p = { sizeof(idt) - 1, (uint32_t)&idt };

    // Set IDT gates (add more interrupt handlers here as needed)
    set_idt_gate(0, (uint32_t)isr0);   // Divide by Zero
    set_idt_gate(6, (uint32_t)isr6);   // Invalid Opcode
    set_idt_gate(13, (uint32_t)isr13); // General Protection Fault
    set_idt_gate(14, (uint32_t)isr14); // Page Fault

    // Load the IDT using the assembly function
    load_idt(&idt_p);
}

void isr0_handler(void) {
    printf("Divide by Zero Exception!\n", RED_ON_BLACK_WARNING);
}

void isr6_handler(void) {
    printf("Invalid Opcode Exception!\n", RED_ON_BLACK_WARNING);
}

void isr13_handler(void) {
    printf("General Protection Fault!\n", RED_ON_BLACK_WARNING);
}

void isr14_handler(void) {
    printf("Page Fault Exception!\n", RED_ON_BLACK_WARNING);
}
