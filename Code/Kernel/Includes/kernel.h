#ifndef _KERNEL_H_
#define _KERNEL_H_

#define KERNEL_HIGH_BASE   0xC0000000
#define KERNEL_LOW_BASE    0x00000000
#define KERNEL_OFFSET_DIFF (KERNEL_HIGH_BASE - KERNEL_LOW_BASE)
#define DELAY_CURSOR_SIGNAL 50

#include <file_system.h>
#include <math.h>
#include <render.h>
#include <ata.h>
#include <shell.h>
#include <time.h>
#include <keyboard.h>
#include <scheduler.h>
#include <PIC.h>
#include <PIT.h>
#include <PCB.h>
#include <memory_manager.h>
#include <test.h>
#include <bitmap.h>
#include <memory_manager.h>
#include <detect_memory.h>
#include <paging.h>
#include <ctype.h>
#include <vga.h>
#include <stdint.h>
#include <io.h>
#include <stdlib.h>
#include <stdarg.h>
#include <idt.h>
#include <gdt.h>

extern uint32_t pagesAllocated;
extern uintptr_t idle_stack_pointer; 
extern uint16_t keyboard_cursor_position;

#endif /* _KERNEL_H_ */