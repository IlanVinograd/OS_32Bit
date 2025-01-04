#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>

void outPort(uint16_t port, uint8_t val);
uint8_t inPort(uint16_t port);
uint16_t inPort16(uint16_t port);
void outPort16(uint16_t port, uint16_t val);
void wait(void);

#endif /* _IO_H_ */