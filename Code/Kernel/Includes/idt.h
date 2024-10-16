#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include <vga.h>

struct idt_entry {
    uint16_t base_low;     // Lower 16 bits of the ISR address
    uint16_t sel;          // Kernel segment selector
    uint8_t  always0;      // This must always be zero
    uint8_t  flags;        // Flags (type, privilege level, present)
    uint16_t base_high;    // Upper 16 bits of the ISR address
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;        // Limit (size of IDT - 1)
    uint32_t base;         // Base address of the IDT
} __attribute__((packed));

void init_idt(void);
void set_idt_gate(int n, uint32_t handler);

void isr0_handler(void);
void isr6_handler(void);
void isr13_handler(void);
void isr14_handler(void);

#endif