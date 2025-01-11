#include "../Includes/io.h"

void outPort(uint16_t port, unsigned char val){
    __asm__("outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

uint8_t inPort(uint16_t port){
    unsigned char returnVal;
    __asm__ __volatile__("inb %1, %0" : "=a"(returnVal) : "Nd"(port) : "memory");
    return returnVal;
}

uint16_t inPort16(uint16_t port) {
    uint16_t returnVal;
    __asm__ __volatile__("inw %1, %0" : "=a"(returnVal) : "Nd"(port) : "memory");
    return returnVal;
}

void outPort16(uint16_t port, uint16_t val) {
    __asm__("outw %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

void wait(void){
    outPort (0x80, 0);
}