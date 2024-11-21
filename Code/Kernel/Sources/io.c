#include "../Includes/io.h"

void outPort(uint16_t port, unsigned char val){
    __asm__("outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

uint8_t inPort(uint16_t port){
    unsigned char returnVal;
    __asm__ __volatile__("inb %1, %0" : "=a"(returnVal) : "Nd"(port) : "memory");
    return returnVal;
}

void wait(void){
    outPort (0x80, 0);
}