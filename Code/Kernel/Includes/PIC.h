#ifndef _PIC_H_
#define _PIC_H_

#include <stdint.h>
#include <idt.h>
#include <PIT.h>
#include <scheduler.h>
#include <io.h>
#include <memory_manager.h>

void pic_init(void);
void pic_send_eoi(uint8_t irq);

#endif /* _PIC_H_ */