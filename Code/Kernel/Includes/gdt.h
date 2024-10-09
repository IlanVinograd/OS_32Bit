#ifndef _GDT_H_
#define _GDT_H_

#include <stdint.h>
#include <string.h>

// GDT entry structure
struct gdt_entry {
    uint16_t limit_low;     // Lower 16 bits of the limit
    uint16_t base_low;      // Lower 16 bits of the base
    uint8_t  base_middle;   // Next 8 bits of the base
    uint8_t  access;        // Access flags
    uint8_t  granularity;   // Granularity and flags
    uint8_t  base_high;     // Last 8 bits of the base
} __attribute__((packed));

// GDT pointer structure
struct gdt_ptr {
    uint16_t limit;         // Limit of the GDT
    uint32_t base;          // Base address of the GDT
} __attribute__((packed));

// TSS entry structure
struct tss_entry {
    uint32_t prev_tss;      // Previous TSS (if using hardware task switching)
    uint32_t esp0;          // Stack pointer for ring 0
    uint32_t ss0;           // Stack segment for ring 0
    // Additional fields can be zero
    uint32_t reserved[23];  // Reserved fields to make the structure size correct
} __attribute__((packed));

// Externally accessible functions
void init_gdt();
void init_segments();
void gdt_set_entry(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

// Externally accessible variables
extern void lgdt(struct gdt_ptr* gdtp);
extern uint32_t phys_stack_top;
extern struct gdt_entry gdt[6];
extern struct gdt_ptr   gdtp;
extern struct tss_entry tss;
extern uint32_t stack_top;

#endif /* _GDT_H_ */