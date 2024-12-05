#include "../Includes/PIT.h"

volatile uint32_t tick_count = 0;
extern char* osVersion;

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

    pit_update_system_time();

    // Fetch current RTC time
    RTC_Time current_time;
    read_rtc(&current_time);

    setCursorPosition(0, 0);
    printf("OS Version: (%s) Bitmap Pages ( %d / %d ) (TASKS: %d) - PIT Ticks - %d\n", COLOR_BLUE_ON_WHITE, osVersion, pagesAllocated, NUM_PAGES, nowTasks, tick_count);
    
    setCursorPosition(1, 0);
    printf("Date: %04d-%02d-%02d Time: %02d:%02d:%02d   \n",
           COLOR_BLUE_ON_WHITE, 
           current_time.year, current_time.month, current_time.day, 
           current_time.hour, current_time.minute, current_time.second);

    pic_send_eoi(0);
    // Don't need to lock the scheduler from within the interrupt handler
    schedule();
}

uint32_t get_tick_count(void) {
    return tick_count;
}