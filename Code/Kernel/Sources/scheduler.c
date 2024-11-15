#include "../Includes/scheduler.h"

extern void switch_to_task_handler(task* current, task* next);
extern task* current;

void schedule() {
    if (!current) {
        return;
    }

    if (current->state == TERMINATED) {
        task* terminated_task = current;
        current = current->next;
        remove_task(terminated_task);
    }

    task* next_task = current;
    do {
        next_task = next_task->next;
    } while (next_task->state != READY && next_task != current);

    if (next_task == current && current->state != READY) {
        // No READY tasks, idle state
        printf("No tasks ready. Entering idle state...\n", RED_ON_BLACK_WARNING);
        return;
    }

    setCursorPosition(15, 0);
    printf("   Prev process: %d | sp: %p | cp: %p\n", YELLOW_ON_BLACK_CAUTION, current->pid,current->sp,current->pc);

    // Perform context switch
    set_task_state(current, READY);
    task* prev = current;
    current = next_task;
    set_task_state(current, RUNNING);
    switch_to_task_handler(prev, next_task); // Giving Page Fault.

    setCursorPosition(16, 0);
    printf("Current process: %d | sp: %p | cp: %p", YELLOW_ON_BLACK_CAUTION, current->pid,current->sp,current->pc );
}

void init_scheduler() {
    if (!current) {
        printf("Scheduler initialization failed: No tasks available.\n", RED_ON_BLACK_WARNING);
        return;
    }

    current->state = RUNNING;
}

void yield(){
    //not in use currently.
}