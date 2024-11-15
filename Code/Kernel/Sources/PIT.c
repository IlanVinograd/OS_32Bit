#include "../Includes/PIT.h"

volatile uint32_t tick_count = 0;

void pit_init(uint32_t frequency) {
    uint32_t divisor = PIT_FREQUENCY / frequency;

    outPort(PIT_COMMAND, 0x36);
    outPort(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outPort(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));
}

void pit_handler(void) {
    __asm__ __volatile__("cli");

    tick_count++;
    setCursorPosition(0, 25);
    printf("Bitmap state: Pages ( %d / %d ) - PIT Ticks - %d \n", COLOR_BLACK_ON_WHITE, pagesAllocated, NUM_PAGES, tick_count);

    if (tick_count % 10 == 0) {
        schedule();
    }

    pic_send_eoi(0);

    __asm__ __volatile__("sti");
}

uint32_t get_tick_count(void) {
    return tick_count;
}