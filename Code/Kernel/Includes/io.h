#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>

void outPort(uint16_t port, uint8_t val);
uint8_t inPort(uint16_t port);
void wait(void);

#endif /* _IO_H_ */