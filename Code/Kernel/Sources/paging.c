#include "../Includes/paging.h"

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

// Set up the first page table, first 4MB of memory (identity mapping)
void init_paging() {
    for (int i = 0; i < 1024; i++) {
        first_page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_WRITABLE;

    // Map virtual address 0xC0000000 to physical address 0x100000
    page_directory[768] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_WRITABLE;

    load_page_directory((uint32_t)page_directory);

    // Enable paging by setting the PG bit in CR0
    enable_paging();
}

void load_page_directory(uint32_t page_directory_address) {
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_directory_address));
}

void enable_paging() {
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;  // Set the paging (PG) bit
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
}