#include "paging.h"

uint32_t page_directory[1024] __attribute__((aligned(4096), section(".lowdata")));
uint32_t first_page_table[1024] __attribute__((aligned(4096), section(".lowdata")));
uint32_t new_page_table[1024] __attribute__((aligned(4096), section(".lowdata")));

static __attribute__((section(".lowtext"))) void low_load_page_directory(uint32_t page_directory_address) {
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_directory_address));
}

static __attribute__((section(".lowtext"))) void low_enable_paging(void) {
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;  // Set the paging (PG) bit
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
}

// Set up the first page table, first 4MB of memory (identity mapping)
// This function must be called from low memory, not high!
__attribute__((section(".lowtext"))) void init_paging(void) {
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

    /* Map 0xC0000000-0xC03FFFFF to 0x00000000-0x003FFFFF. */
    for (int i = 0; i < 1024; i++) {
        new_page_table[i] = (0x000000 + i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    // Load the page directory address into the CR3 register
    low_load_page_directory((uint32_t)page_directory);

    // Enable paging by setting the PG bit in CR0
    low_enable_paging();
}

// The following functions can be called from the higher half

// Get contents of CR3 register
uint32_t get_cr3(void) {
    uint32_t cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

// Set CR3 register
void set_cr3(uint32_t page_directory_address) {
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_directory_address) : "memory");
}

// Flush entire TLB
void flush_tlb(void) {
    // Reloading CR3 with itself will flush the entire TLB
    set_cr3(get_cr3());
}

// Load CR3 with
void load_page_directory(uint32_t page_directory_address) {
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_directory_address) : "memory");
}

// Remove first 4MiB identity mapping
//   0x00000000-0x00000000 to 0x00000000-0x003FFFFF.
void remove_first_4MiB_mapping(void) {
    page_directory[0] = ((uint32_t)0x00000000);
    flush_tlb();
}

