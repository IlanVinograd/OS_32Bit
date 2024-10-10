#include<gdt.h>

// Declare the GDT with 6 entries
struct gdt_entry gdt[6];
struct gdt_ptr   gdtp;
struct tss_entry tss;

void lgdt(struct gdt_ptr* gdtp) {
    __asm__ ("lgdt %0" :: "m"(*gdtp) : "memory");
}

void init_gdt() {
    // Setup the GDT pointer
    gdtp.limit = (sizeof(struct gdt_entry) * 6) - 1;
    gdtp.base  = (uint32_t)&gdt;

    // Null descriptor (required)
    memset(&gdt[0], 0, sizeof(struct gdt_entry));

    // Kernel code segment
    gdt_set_entry(1, 0x00000000, 0xFFFFFFFF, 0x9A, 0xCF);

    // Kernel data segment
    gdt_set_entry(2, 0x00000000, 0xFFFFFFFF, 0x92, 0xCF);

    // User code segment
    gdt_set_entry(3, 0x00000000, 0xFFFFFFFF, 0xFA, 0xCF);

    // User data segment
    gdt_set_entry(4, 0x00000000, 0xFFFFFFFF, 0xF2, 0xCF);

    // TSS segment
    uint32_t base = (uint32_t)&tss;
    uint32_t limit = sizeof(tss) - 1;

    gdt_set_entry(5, base, limit, 0x89, 0x40);  // Access byte 0x89 for TSS, granularity 0x40

    // Flush old GDT and load the new one
    lgdt(&gdtp);

    // Initialize the TSS
    memset(&tss, 0, sizeof(tss));
    tss.ss0  = 0x10;      // Kernel data segment selector
    tss.esp0 = (uint32_t)esp0_stack_top;  // Stack pointer for ring 0 transitions

    // Load the TSS
    __asm__ ("ltr %0" : : "r" ((uint16_t)0x28));  // TSS segment selector (5th entry, index 5*8=0x28)
}

void gdt_set_entry(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access      = access;
}

void init_segments() {
    // Reload code segment
    __asm__ volatile (
        "jmp $0x08, $.flush_cs\n"
        ".flush_cs:\n" ::: "memory"
    );

    // Reload data segment registers
    __asm__ volatile (
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        : : : "ax", "memory"
    );
}
