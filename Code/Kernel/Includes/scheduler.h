#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <PCB.h>
#include <vga.h>
#include <gdt.h>
#include <stdint.h>
#include <PIT.h>

//void switch_context(task* current_task, task* next_task);
void schedule();
void init_scheduler();
void free_scheduler(void);
void yield();

#endif
