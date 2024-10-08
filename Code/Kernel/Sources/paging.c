#include "../Includes/paging.h"

uint32_t page_directory[1024] __attribute__((aligned(4096), section(".lowdata")));
uint32_t first_page_table[1024] __attribute__((aligned(4096), section(".lowdata")));
uint32_t new_page_table[1024] __attribute__((aligned(4096), section(".lowdata")));

// Set up the first page table, first 4MB of memory (identity mapping)
__attribute__((section(".lowtext"))) void init_paging() {
    // Mark the rest of the page directory as not present
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0;
    }

    // Add the page table to the page directory
    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_WRITABLE;
    page_directory[768] = ((uint32_t)new_page_table) | PAGE_PRESENT | PAGE_WRITABLE;

    for (int i = 0; i < 1024; i++) {
        first_page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    for (int i = 0; i < 1024; i++) {
        new_page_table[i] = (0x100000 + i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    // Load the page directory address into the CR3 register
    load_page_directory((uint32_t)page_directory);

    // Enable paging by setting the PG bit in CR0
    enable_paging();
}

__attribute__((section(".lowtext"))) void load_page_directory(uint32_t page_directory_address) {
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_directory_address));
}

__attribute__((section(".lowtext"))) void enable_paging() {
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;  // Set the paging (PG) bit
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
}
