#include "../Includes/scheduler.h"

extern void switch_to_task_handler(task* current_task, task* next_task);

extern task* current;

void schedule() {
    if (!current) return;

    if (current->state == TERMINATED) {
        task* terminated_task = current;
        current = current->next;
        remove_task(terminated_task);
    }

    task* next_task = current->next;
    while (next_task->state != READY && next_task != current) {
        next_task = next_task->next;
    }
    setCursorPosition(5, 5);
    printf("Current process: %d | sp: %p | cp: %p", YELLOW_ON_BLACK_CAUTION, current->pid,current->sp,current->pc);

    current = next_task;
    current->state = RUNNING;

    setCursorPosition(7, 5);
    printf("Current process: %d | sp: %p | cp: %p", YELLOW_ON_BLACK_CAUTION, current->pid,current->sp,current->pc );
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
    task* next_task = current->next;

    while (next_task->state != READY && next_task != current) {
        next_task = next_task->next;
    }

    if (next_task != current) {
        task* previous_task = current;
        current = next_task;
        switch_to_task_handler(previous_task, current);
    }

    current->state = RUNNING;
    setCursorPosition(5, 5);
    printf("Current process: %d | sp: %p | cp: %p", YELLOW_ON_BLACK_CAUTION, current->pid, current->sp, current->pc);
}