#ifndef _PAGING_H_
#define _PAGING_H_

#include <stdint.h>
#include <io.h>

extern uint32_t page_directory[1024];
extern uint32_t first_page_table[1024];

#define PAGE_SIZE 4096  // Size of each page (4KB)

// Define bit flags for the entries in page directory/table
#define PAGE_PRESENT    0x1  // Page is present in memory
#define PAGE_WRITABLE   0x2  // Page is writable
#define PAGE_USER       0x4  // Page is accessible in user mode

// Page directory and page table entry structure
typedef uint32_t page_directory_entry_t;
typedef uint32_t page_table_entry_t;

void init_paging();
void load_page_directory(uint32_t page_directory_address);
void enable_paging();

#endif /* _PAGING_H_ */