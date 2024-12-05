#ifndef _PIT_H_
#define _PIT_H_

#include <time.h>
#include <stdint.h>
#include <io.h>
#include <vga.h>
#include <idt.h>
#include <PIC.h>
#include <scheduler.h>
#include <PCB.h>

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43
#define PIT_FREQUENCY 1193182 // Base frequency of PIT in Hz
#define UTS 2 // Universal Time

extern uint32_t pagesAllocated;

void pit_init(uint32_t frequency);
void pit_handler(void);
uint32_t get_tick_count(void);

#endif /* _PIT_H_ */