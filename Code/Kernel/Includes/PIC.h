#ifndef _PIC_H_
#define _PIC_H_

#include <stdint.h>
#include <idt.h>
#include <PIT.h>
#include <scheduler.h>
#include <io.h>
#include <memory_manager.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

void pic_init(void);
void pic_send_eoi(uint8_t irq);

#endif /* _PIC_H_ */