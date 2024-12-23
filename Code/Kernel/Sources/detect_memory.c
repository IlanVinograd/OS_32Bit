#include "../Includes/detect_memory.h"

void init_memory_map() {
    memory_map_entry_t* mem_map = (memory_map_entry_t*)MEMORY_MAP_VADDR;

    uint8_t count = *((uint8_t*)COUNT_ENTRIES_VADDR);
    printf("Memory map entry count: %d\n\n", COLOR_BLACK_ON_WHITE, count);

    for (int i = 0; i < count; i++) {
        if(mem_map[i].type == 1){
            printf("Usable memory: Base: 0x%llx, Length: 0x%llx, type: %d\n", GREEN_ON_BLACK_SUCCESS, mem_map[i].base_addr, mem_map[i].length, mem_map[i].type);
        } else {
            printf("Usable memory: Base: 0x%llx, Length: 0x%llx, type: %d\n", RED_ON_BLACK_WARNING, mem_map[i].base_addr, mem_map[i].length, mem_map[i].type);
        }
    }
}