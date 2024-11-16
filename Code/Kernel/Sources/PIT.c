#include "../Includes/PIT.h"

volatile uint32_t tick_count = 0;

void pit_init(uint32_t frequency) {
    uint32_t divisor = PIT_FREQUENCY / frequency;

    outPort(PIT_COMMAND, 0x36);
    outPort(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outPort(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));
}

void pit_handler(void) { // each 10ms will be interrupt.
    // IRQ handlers ar set as interrupt gates in the IDT so interrupts
    // will always be off a this point as a result. No CLI necessary.
    tick_count++;

    setCursorPosition(24, 0);
    printf("Bitmap state: Pages ( %d / %d ) (TASKS: %d) - PIT Ticks - %d \n", COLOR_BLACK_ON_WHITE, pagesAllocated, NUM_PAGES, nowTasks, tick_count);
    pic_send_eoi(0);
    lock_scheduler();
    schedule();
    unlock_scheduler();
}

uint32_t get_tick_count(void) {
    return tick_count;
}
