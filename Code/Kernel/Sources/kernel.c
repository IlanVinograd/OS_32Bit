#include "../Includes/kernel.h"

volatile uint32_t count = 0;

void task1_entry() {
    setCursorPosition(15, 0);
    printf("Test1 Start - %d\n", GREEN_ON_BLACK_SUCCESS, count);
    for (int i = 1; i <= 10000; i++) {
        count++;
        setCursorPosition(13, 0);
        printf("loop count -> %d", COLOR_BLINKING_YELLOW, count);
    }
    setCursorPosition(16, 0);
    printf("Test1 Finished - %d\n", GREEN_ON_BLACK_SUCCESS, count);
    set_task_state(current, TERMINATED);
}

void task2_entry() {
    setCursorPosition(17, 0);
    printf("Test2 Start - %d\n", GREEN_ON_BLACK_SUCCESS, count);
    for (int i = 1; i <= 10000; i++) {
        count++;
        setCursorPosition(13, 0);
        printf("loop count (Task2) -> %d", COLOR_BLINKING_YELLOW, count);
    }
    setCursorPosition(18, 0);
    printf("Test2 Finished - %d\n", GREEN_ON_BLACK_SUCCESS, count);
    set_task_state(current, TERMINATED);
}

void idle_task() {
    setCursorPosition(1, 0);
    printf("DONE\n",GREEN_ON_BLACK_SUCCESS); // Idle state
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

    setCursorPosition(20, 0);
    create_task((uintptr_t)task1_entry);
    create_task((uintptr_t)task2_entry);

    setCursorPosition(18, 0);
    print_task_and_count();
    pit_init(100);   // each 10ms will be interrupt.
    __asm__("sti");  // Enable global interrupts.

    idle_task();
    while (1) __asm__ ("hlt");  // Loop indefinitely
}