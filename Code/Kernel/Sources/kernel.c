#include "../Includes/kernel.h"
#include "../Includes/PCB.h"

//volatile uint32_t count = 0;

void task1_entry() {
    uint32_t count1 = 0;
    setCursorPosition(15, 0);
    printf("Test1 Start - %d\n", GREEN_ON_BLACK_SUCCESS, count1);

    for (int i = 1; i <= 20000; i++) {
//    for (int i = 1; i <= 10000000; i++) {
        count1++;
        __asm__("cli");
        setCursorPosition(10, 0);
        printf("loop count 1 -> %d", COLOR_BLINKING_YELLOW, count1);
        __asm__("sti");
    }
    setCursorPosition(15, 30);
    printf("Test1 Finished - %d\n", GREEN_ON_BLACK_SUCCESS, count1);
    set_task_state(current, TERMINATED);
    yield();
}

void task2_entry() {
    uint32_t count2 = 0;
    setCursorPosition(16, 0);
    printf("Test2 Start - %d\n", GREEN_ON_BLACK_SUCCESS, count2);
//    for (int i = 1; i <= 10000000; i++) {
    for (int i = 1; i <= 10000; i++) {
        count2++;
        __asm__("cli");
        setCursorPosition(11, 0);
        printf("loop count 2 -> %d", COLOR_BLINKING_YELLOW, count2);
        __asm__("sti");
    }

    setCursorPosition(16, 30);
    printf("Test2 Finished - %d\n", GREEN_ON_BLACK_SUCCESS, count2);
    set_task_state(current, TERMINATED);
    yield();
}

void task3_entry() {
    uint32_t count3 = 0;
    setCursorPosition(17, 0);
    printf("Test3 Start - %d\n", GREEN_ON_BLACK_SUCCESS, count3);

    for (int i = 1; i <= 30000; i++) {
//    for (int i = 1; i <= 10000000; i++) {
        count3++;
        __asm__("cli");
        setCursorPosition(12, 0);
        printf("loop count 3 -> %d", COLOR_BLINKING_YELLOW, count3);
        __asm__("sti");
    }
    setCursorPosition(17, 30);
    printf("Test1 Finished - %d\n", GREEN_ON_BLACK_SUCCESS, count3);
    set_task_state(current, TERMINATED);
    yield();
}

void task_main() {
    while (nowTasks > 2) {
        yield();  // Allow other tasks to run
    }

    setCursorPosition(18, 0);
    printf("DONE\n", GREEN_ON_BLACK_SUCCESS);

    while (1) {
        __asm__("hlt");  // Halt indefinitely
    }
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

    // Create sample work tasks
    setCursorPosition(19,0);
    create_task((uintptr_t)task1_entry);
    create_task((uintptr_t)task2_entry);
    create_task((uintptr_t)task3_entry);
    create_task((uintptr_t)task_main);

    setCursorPosition(0,24);
    print_task_and_count();
    __asm__("sti");

    while (1) __asm__ ("hlt");  // Loop indefinitely
}
