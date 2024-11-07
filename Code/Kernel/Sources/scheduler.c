#include "../Includes/scheduler.h"

extern task* current;

void schedule() {
    if (!current) return;

    // Save the current task's stack pointer if it is running
    if (current->state == RUNNING) {
        __asm__ volatile("movl %%esp, %0" : "=m"(current->sp));
        current->state = READY;
    }

    // Find the next task that is ready to run
    task* next_task = current->next;
    while (next_task->state != READY && next_task != current) {
        next_task = next_task->next;
    }
    setCursorPosition(2, 30);
    printf("Current process: %d | sp: %p | cp: %p", COLOR_BLACK_ON_WHITE, current->pid,current->sp,current->pc);
    current = next_task;
    current->state = RUNNING;

    // Display the current process PID
    setCursorPosition(1, 30);
    printf("Current process: %d | sp: %p | cp: %p", COLOR_BLACK_ON_WHITE, current->pid,current->sp,current->pc );

    // Set up the TSS for the new task's stack
    tss.esp0 = current->sp;
    tss.ss0 = 0x10; // Kernel data segment selector

    // Restore the new task's stack pointer and jump to its program counter
    __asm__ volatile(
        "movl %0, %%esp\n"
        "jmp *%1\n"
        : : "m"(current->sp), "m"(current->pc)
    );

    if (current->state == TERMINATED) {
        remove_task(current);
    }
}

void init_scheduler() {
    if (!current) {
        printf("Scheduler initialization failed: No tasks available.\n", RED_ON_BLACK_WARNING);
        return;
    }

    current->state = RUNNING;
    pit_init(1);  // Set the PIT to 100Hz, or every 10ms
    __asm__("sti");  // Enable global interrupts
}

void yield() {
    __asm__ ("int $0x20");  // Trigger PIT interrupt
}