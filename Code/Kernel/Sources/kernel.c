#include "../Includes/kernel.h"

volatile uint32_t tasks_finished = 0;

void cursor_signal(){
    volatile uint32_t ticks_last = 0;
    bool_t cursorSignal = true;
    unlock_scheduler();

    while(true) {
        volatile uint32_t ticks_now = get_tick_count();

        if(ticks_now - ticks_last >= DELAY_CURSOR_SIGNAL){
            cursorSignal = !cursorSignal;
            ticks_last = ticks_now;

            blinkCursor(cursorSignal);
        }
        yield();
    }
}

void rtc_sync_task() {
    unlock_scheduler();
    while (true) {
        synchronize_rtc_with_system_time();
        yield();
    }
}

void initial_jobs() {
    unlock_scheduler();

    // Set the initial keyboard cursor position
    init_shell();

    create_task((uintptr_t)cursor_signal);
    create_task((uintptr_t)rtc_sync_task);

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
    initScreen("1.0");
    enable_keyboard();
    ata_initialize(ATA_PRIMARY_IO, ATA_SLAVE);
    init_fs();
    // Initialize the scheduler and enable interrupts
    init_scheduler();
    __asm__("sti");

    // Start the initial jobs task
    create_task((uintptr_t)initial_jobs);
    task_main(); // Do main task. task_main will not return
}