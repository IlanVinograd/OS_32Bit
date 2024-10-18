#include <paging.h>
#include <kernel.h>

// Load CR3 with Page Directory address
void load_page_directory(uintptr_t page_directory_address) {
    __asm__ ("mov %0, %%cr3" :: "r"(page_directory_address) : "memory");
}

// Enable paging
static void enable_paging(void) {
    uint32_t cr0;
    __asm__ __volatile__ ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;  // Set the paging (PG) bit
    __asm__ ("mov %0, %%cr0" :: "r"(cr0));
}

// Set up the first page table, first 4MB of memory (identity mapping)
// This function must be called from low memory, not high!
void init_paging(void) {

    // Mark the rest of the page directory as not present
    for (int i = 0; i < 1024; i++) {
        page_directory_low[i] = 0;
    }

    // Add the page table to the page directory
    page_directory_low[0]   = ((uintptr_t)first_page_table_low) | PAGE_PRESENT | PAGE_WRITABLE;
    page_directory_low[768] = ((uintptr_t)new_page_table_low) | PAGE_PRESENT | PAGE_WRITABLE;
    page_directory_low[769] = ((uintptr_t)new_page_alloc_table_high) | PAGE_PRESENT | PAGE_WRITABLE; // page table for page_alloc

    for (int i = 0; i < 1024; i++) {
        first_page_table_low[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    /* Map 0xC0000000-0xC03FFFFF to 0x00000000-0x003FFFFF. */
    for (int i = 0; i < 1024; i++) {
        new_page_table_low[i] = (0x000000 + i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    /* Map 0xC0400000-0xC07FFFFF */
    for (int i = 0; i < 1024; i++) {
        new_page_alloc_table_high[i] = (0xC0400000 + i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    // Load the page directory address into the CR3 register
    load_page_directory((uintptr_t)page_directory_low);

    // Enable paging by setting the PG bit in CR0
    enable_paging();
}

// The following functions can be called from the higher half

// Get contents of CR3 register
uintptr_t get_cr3(void) {
    uintptr_t cr3;
    __asm__ __volatile__ ("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

// Set CR3 register
void set_cr3(uintptr_t page_directory_address) {
    __asm__ ("mov %0, %%cr3" :: "r"(page_directory_address) : "memory");
}

// Flush entire TLB
void flush_tlb(void) {
    // Reloading CR3 with itself will flush the entire TLB
    set_cr3(get_cr3());
}

// Remove first 4MiB identity mapping
// from 0x00000000-0x00000000 to 0x00000000-0x003FFFFF.
void remove_first_4MiB_mapping(void) {
    page_directory_low[0] = ((uintptr_t)0x00000000);
    flush_tlb();
}

