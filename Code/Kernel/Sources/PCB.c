#include "../Includes/PCB.h"

task* current = NULL;   // Pointer to the currently running task
static uint32_t currentPID = 0;
uint32_t nowTasks = 0;

task* create_task(uintptr_t task_entry_function) {
    task* new_task = (task*)malloc(sizeof(task));
    if (new_task) {
        new_task->pid = new_pid();
        new_task->state = READY;
        new_task->pc = task_entry_function;
        new_task->sp = (uintptr_t *)malloc(STACK_SIZE) + STACK_SIZE;
        new_task->fsp = new_task->sp; // add commment

//      This would apply to a user task
//      new_task->esp0 = (uintptr_t)malloc(STACK_SIZE) + STACK_SIZE;
//      new_task->ss0 = 0x10;

        // Initialize task stack. These entries are reverse to the order
        // that switch_to_task pops them when it returns.
        *(--new_task->sp) = task_entry_function;   // EIP
        *(--new_task->sp) = 0x202;                 // Eflags - IF=1, BIT1=1 (reserved)
        *(--new_task->sp) = 0;                     // EBX
        *(--new_task->sp) = 0;                     // ESI
        *(--new_task->sp) = 0;                     // EDI
        *(--new_task->sp) = 0;                     // EBP

        printf("Creating Task PID: %d | PC: %p | SP: %p | State: %d\n", COLOR_BLACK_ON_WHITE,
               new_task->pid, new_task->pc, new_task->sp, new_task->state);

        // Insert task in circular list
        if (!current) {
            current = new_task;
            new_task->next = current;
        } else {
            task* temp = current;
            while (temp->next != current) {
                temp = temp->next;
            }
            temp->next = new_task;
            new_task->next = current;
        }
        nowTasks++;
    }
    return new_task;
}

void remove_task(task* task_terminate) {
    if (!current || !task_terminate) return;

    if (current == task_terminate && current->next == current) {
        free((uintptr_t *)(task_terminate->fsp - STACK_SIZE));
        free(task_terminate);
        current = NULL;
        nowTasks--;
        return;
    }

    task* temp = current;
    task* prev = NULL;

    do {
        if (temp == task_terminate) {
            if (prev) {
                prev->next = temp->next;
            }
            if (current == task_terminate) {
                current = temp->next;
            }

            free((uintptr_t *)(task_terminate->fsp - STACK_SIZE));
            free(task_terminate);
            nowTasks--;
            return;
        }

        prev = temp;
        temp = temp->next;
    } while (temp != current);
}

void print_task_and_count() {
    setCursorPosition(0, 0);
    printf("Tasks now | %d |\n", COLOR_BLACK_ON_WHITE, nowTasks);
    task* temp = current;

    if (!current) {
        printf("No tasks in the list.\n", COLOR_BLACK_ON_WHITE);
        return;
    }

    do {
        printf("PID: %d -> ", COLOR_BLINKING_YELLOW, temp->pid);
        temp = temp->next;
    } while (temp != current);
}

uint32_t new_pid() {
    currentPID = (currentPID >= MAX_PIDs) ? 0 : currentPID + 1; // Increment currentPID and wrap around if it exceeds MAX_PIDs
    return currentPID;
}

void set_task_state(task* task, enum State state) {
    if (task) {
        task->state = state;
    }
}
