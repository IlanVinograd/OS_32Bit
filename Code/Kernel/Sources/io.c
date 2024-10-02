#include "../includes/io.h"

void outPort(uint16_t port, unsigned char val){
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t inPort(uint16_t port){
    unsigned char returnVal;
    __asm__ __volatile__("inb %1, %0" : "=a"(returnVal) : "Nd"(port));
    return returnVal;
}

void wait(void){
    __asm__ __volatile__("outb %%al, $0x80" : : "a"(0) );
}