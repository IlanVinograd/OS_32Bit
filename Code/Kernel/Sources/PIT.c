#include "../Includes/PIT.h"

volatile uint32_t tick_count = 0;
extern char* osVersion;
extern task* current;

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
    printf(" [ %04d / %02d / %02d ] OS Version (", COLOR_BLUE_ON_WHITE,
           current_time.year, current_time.month, current_time.day);
    printf("%s", COLOR_BLUE_ON_LIGHT_BROWN, osVersion);
    printf(")             BP ( ",COLOR_BLUE_ON_WHITE);
    printf("%d", COLOR_BLUE_ON_LIGHT_BROWN, pagesAllocated);
    printf(" / %d ) (TASKS: ",COLOR_BLUE_ON_WHITE, NUM_PAGES);
    printf("%d",COLOR_BLUE_ON_LIGHT_BROWN, nowTasks);
    printf(" )", COLOR_BLUE_ON_WHITE);

    setCursorPosition(1, 0);
    printf(" [   %02d / %02d / %02d ] ", COLOR_BLUE_ON_WHITE, current_time.hour + UTS, current_time.minute, current_time.second);
    printf("Ticks: ", COLOR_BLUE_ON_WHITE);
    printf("%d       ", COLOR_BLUE_ON_LIGHT_BROWN, tick_count);

    setCursorPosition(1, 49);
    printf("Mem: ", COLOR_BLUE_ON_WHITE);


    pic_send_eoi(0);
    // Don't need to lock the scheduler from within the interrupt handler
    schedule();
}

uint32_t get_tick_count(void) {
    return tick_count;
}