#include "../Includes/scheduler.h"

extern void switch_to_task(task *next_task);

extern task* current;

void schedule() {
    if (!current) {
        return;
    }

    // Find the next task that is ready to run
    task* next_task = current->next;

    if (next_task->state == TERMINATED) {
        remove_task(next_task);
        next_task = current->next;
    }

    while (next_task->state != READY && next_task != current) {
        next_task = next_task->next;
    }

    setCursorPosition(2, 30);
    printf("Current process: %d | sp: %p | cp: %p", COLOR_BLACK_ON_WHITE, current->pid,current->sp,current->pc);
    switch_to_task(next_task);

    // Display the current process PID
    setCursorPosition(1, 30);
    printf("Current process: %d | sp: %p | cp: %p", COLOR_BLACK_ON_WHITE, current->pid,current->sp,current->pc );
}

void init_scheduler(void) {
    current = (task*)malloc(sizeof(task));
    // Create a PCB for the main task
    if (current) {
        current->pid = new_pid();
        current->state = READY;
        current->flags = 0;
        current->pc = 0;
        current->sp = 0;
        current->fsp = current->sp;
        current->esp0 = 0;
        current->ss0 = 0;
        current->next = current;
        nowTasks++;
    }


    pit_init(1);  // Set the PIT to 100Hz, or every 10ms
}


void yield (void) {
    schedule();
}