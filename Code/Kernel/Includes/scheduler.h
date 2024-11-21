#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdint.h>
#include <PCB.h>

struct task;
typedef struct task task;

void schedule(void);
void init_scheduler(void);
void yield(void);

void lock_scheduler(void);
// Call unlock_scheduler at the very start of every task!
void unlock_scheduler(void);

#endif /* _SCHEDULER_H_ */