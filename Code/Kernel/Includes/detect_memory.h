#ifndef _DETECT_MEMORY_H
#define _DETECT_MEMORY_H

#include <stdint.h>
#include <vga.h>

#define MEMORY_MAP_VADDR    0xC0005000
#define COUNT_ENTRIES_VADDR 0xC0004FFE

typedef struct {
    uint64_t base_addr;   // Base address
    uint64_t length;      // Length in bytes
    uint32_t type;        // Memory type (1 = usable, others are reserved)
    uint32_t ACPI;        // Extended ACPI info
} __attribute__((packed)) memory_map_entry_t;

void init_memory_map();

#endif
