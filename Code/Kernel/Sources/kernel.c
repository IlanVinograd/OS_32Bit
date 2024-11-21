#include "../Includes/kernel.h"
#include "../Includes/PCB.h"

//volatile uint32_t count = 0;

volatile uint32_t tasks_finished = 0;

void task1_entry() {
    uint32_t count1 = 0;
    unlock_scheduler();
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
    tasks_finished++;
    yield();
}

void task2_entry() {
    uint32_t count2 = 0;
    unlock_scheduler();
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
    tasks_finished++;
    yield();
}

void task3_entry() {
    uint32_t count3 = 0;
    unlock_scheduler();
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
    printf("Test3 Finished - %d\n", GREEN_ON_BLACK_SUCCESS, count3);
    set_task_state(current, TERMINATED);
    tasks_finished++;
    yield();
}

void job1_entry() {
    // Create sample work tasks
    unlock_scheduler();
    setCursorPosition(19,0);
    create_task((uintptr_t)task1_entry);
    create_task((uintptr_t)task2_entry);
    create_task((uintptr_t)task3_entry);

    // This job isn't done until the 3 tasks are finished
    while (tasks_finished < 3)
        // Once we perform a check immediately yield to the next task
        yield();

    setCursorPosition(18, 0);
    printf("DONE\n", GREEN_ON_BLACK_SUCCESS);
    set_task_state(current, TERMINATED);
    yield();
}

// Mark the task_main as not returning
__attribute__((noreturn)) void task_main() {
    while (true) {
        // If there are no other tasks besides main running then
        // hlt until next interrupt
        if (nowTasks == 1)
            __asm__("hlt");
        // Otherwise immediately yield to next task
        else
            yield();
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

    // Creates a PCB for the main task (this code) using current stack
    // and initializes the task list
    init_scheduler();
    __asm__("sti");

    setCursorPosition(0,24);
    print_task_and_count();

    // Create an initial counting job to perform.
    // This job will creates 3 subtasks when it starts
    create_task((uintptr_t)job1_entry);

    task_main(); // Do main task. task_main will not return
}
