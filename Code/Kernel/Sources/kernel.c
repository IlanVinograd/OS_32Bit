#include "../Includes/kernel.h"

void _start(void) {
    init_idt();
    
    clearScreen();
    setCursorPosition(0, 0);
    initScreen("0.2");

    int a = 10;
    int b = 0;
    int c = a / b;
    
    while (1);  // Loop indefinitely
}