#ifndef IO_HEADER
#define IO_HEADERs

#include <stdint.h>

void outPort(uint16_t port, uint8_t val);
uint8_t inPort(uint16_t port);
void wait(void);

#endif