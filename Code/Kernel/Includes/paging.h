#ifndef _PAGING_H_
#define _PAGING_H_

#include <stdint.h>
#include <io.h>

extern uintptr_t page_directory_low[1024];
extern uintptr_t first_page_table_low[1024];
extern uintptr_t new_page_table_low[1024];
extern uintptr_t new_page_alloc_table_high[1024];

extern uintptr_t page_directory[1024];
extern uintptr_t first_page_table[1024];
extern uintptr_t new_page_table[1024];

#define PAGE_SIZE 4096  // Size of each page (4KB)

// Define bit flags for the entries in page directory/table
#define PAGE_PRESENT    0x1  // Page is present in memory
#define PAGE_WRITABLE   0x2  // Page is writable
#define PAGE_USER       0x4  // Page is accessible in user mode

// Page directory and page table entry structure
typedef uintptr_t page_directory_entry_t;
typedef uintptr_t page_table_entry_t;

void init_paging(void);
void load_page_directory(uintptr_t page_directory_address);
uint32_t get_cr3(void);
void set_cr3(uintptr_t page_directory_address);
void flush_tlb(void);
void remove_first_4MiB_mapping(void);

#endif /* _PAGING_H_ */