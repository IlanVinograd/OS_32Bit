#include "../Includes/kernel.h"

volatile uint32_t tasks_finished = 0;
extern keyboard_cursor_position;

void keyboard_test_task() {
    unlock_scheduler();
    
    // Wait for keyboard inputs indefinitely (can be improved with more complex logic)
    while (true) {
        __asm__("hlt"); // Halt the CPU until the next interrupt
    }
}

void job1_entry() {
    unlock_scheduler();
    setCursorPosition(1, 0);
    const char *test_message = "Keyboard Test: Type something...";
    printf("%s\n", GREEN_ON_BLACK_SUCCESS, test_message);

    // Set the initial keyboard cursor position
    keyboard_cursor_position = 1 * VGA_COLS + VGA_COLS; // Move to the next space after the message

    create_task((uintptr_t)keyboard_test_task);

    while (true) {
        yield(); // Keep yielding to allow other tasks to run
    }
}

// Mark the task_main as not returning
__attribute__((noreturn)) void task_main() {
    while (true) {
        // If there are no other tasks besides main running then
        // hlt until the next interrupt
        if (nowTasks == 1)
            __asm__("hlt");
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
    initScreen("0.4");
    enable_keyboard();

    // Create the initial task for keyboard testing
    init_scheduler();
    __asm__("sti");

    setCursorPosition(0, 24);
    print_task_and_count();

    // Create the keyboard test task
    create_task((uintptr_t)job1_entry);
    task_main(); // Do main task. task_main will not return
}