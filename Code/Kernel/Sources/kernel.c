#include "../Includes/kernel.h"
#include "../Includes/PCB.h"

volatile uint32_t count = 0;

void task1_entry() {
    setCursorPosition(0, 0);
    printf("Test1 Start - %d\n", GREEN_ON_BLACK_SUCCESS, count);
    for (int i = 1; i <= 100000; i++) {
//    for (int i = 1; i <= 10000000; i++) {
        count++;
        setCursorPosition(10, 0);
        printf("loop count 1 -> %d", COLOR_BLINKING_YELLOW, count);

        if (i % 1000 == 0) yield();
    }
    setCursorPosition(4, 0);
    printf("Test1 Finished - %d\n", GREEN_ON_BLACK_SUCCESS, count);
    set_task_state(current, TERMINATED);
    yield();
}

void task2_entry() {
    setCursorPosition(1, 0);
    printf("Test2 Start - %d\n", GREEN_ON_BLACK_SUCCESS, count);
//    for (int i = 1; i <= 10000000; i++) {
    for (int i = 1; i <= 100000; i++) {
        count++;
        setCursorPosition(11, 0);
        printf("loop count 2 -> %d", COLOR_BLINKING_YELLOW, count);

        if (i % 1000 == 0) yield();
    }
    setCursorPosition(5, 0);
    printf("Test2 Finished - %d\n", GREEN_ON_BLACK_SUCCESS, count);
    set_task_state(current, TERMINATED);
    yield();
}

void _start(void) {
    remove_first_4MiB_mapping();
    init_idt();
    init_gdt();
    init_segments();

    clearScreen();
    setCursorPosition(0, 0);
    init_bitmap();
    pic_init();
    init_free_list();
    init_scheduler();

    // Enable global interrupts
    // IRQ0's handler DOES NOT do task switching in
    // Cooperative multitasking.
    __asm__("sti");

    // Create sample work takss
    create_task((uintptr_t)task1_entry);
    create_task((uintptr_t)task2_entry);

    setCursorPosition(0,24);
    print_task_and_count();

    // This iis the controlling task that waits until
    // All other tasks are completed and then exits.
    while(nowTasks > 1) {
        yield();
    }

    setCursorPosition(24, 0);
    printf("DONE\n",COLOR_BLACK_ON_WHITE);

    while (1) __asm__ ("hlt");  // Loop indefinitely
}
