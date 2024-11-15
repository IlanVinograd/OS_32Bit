#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <PCB.h>

struct task;
typedef struct task task;

void schedule();
void init_scheduler();
void yield();

#endif