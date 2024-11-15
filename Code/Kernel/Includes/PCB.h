#ifndef _PCB_H
#define _PCB_H

#include <stdint.h>
#include <vga.h>

#define MAX_PIDs 4294967295U
#define STACK_SIZE 4096

enum State {
  READY = 0,
  RUNNING = 1,
  WAITING = 2,
  TERMINATED = 3
};

typedef struct task {
    uint32_t pid;          // Process ID
    uintptr_t sp;          // Stack Pointer
    uintptr_t pc;          // Program Counter
    uint32_t state;        // Task state (e.g., READY, RUNNING)
    struct task* next;     // Pointer to the next task in the scheduler's list
} task;

extern task* current;

task* create_task(uintptr_t task_entry_function);
void remove_task(struct task* task_terminate);
void print_task_and_count();
uint32_t new_pid();
void free_task_resources(task* terminated_task);
void set_task_state(task* task, enum State state);

#endif