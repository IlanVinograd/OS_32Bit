#include "../Includes/kernel.h"

void _start(void) {
    clearScreen();
    setCursorPosition(0, 0);
    initScreen("0.2");
    while (1);  // Loop indefinitely
}