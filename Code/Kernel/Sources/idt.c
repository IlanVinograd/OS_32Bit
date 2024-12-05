#include "../Includes/idt.h"

// Declaring IDT array
struct idt_entry idt[256];

// These are external assembly functions, declared but not defined here
extern void isr0();
extern void isr6();
extern void isr13();
extern void isr14();
extern void isr32();

extern void irq0();
extern void irq1();

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

    set_idt_gate(32, (uint32_t)irq0); // IRQ0 (Timer)
    set_idt_gate(33, (uint32_t)irq1); // IRQ1 (Keyboard)

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
    uint32_t faulting_address;
    uint32_t error_code;

    // Get the faulting address from CR2
    __asm__ volatile("mov %%cr2, %0" : "=r" (faulting_address));

    // Retrieve the error code (passed automatically to the handler in some implementations)
    __asm__ volatile("movl 4(%%esp), %0" : "=r"(error_code));

    // Decode the error code to understand the nature of the page fault
    printf("Page Fault Exception!\n", RED_ON_BLACK_WARNING);
    printf("Faulting Address: %p\n",RED_ON_BLACK_WARNING, faulting_address);
    printf("Error Code: 0x%p\n",RED_ON_BLACK_WARNING, error_code);
}

void irq0_handler(void) {
    pit_handler();  // Call the PIT handler to increment the tick count
    pic_send_eoi(0);  // Send EOI to PIC for IRQ0
}

void irq1_handler(void) {
    handle_keyboard_input();
    pic_send_eoi(1);  // Send EOI to PIC for IRQ1
}