#include "../Includes/kernel.h"

void _start(void) {
    uint32_t number = 123456;
    uint16_t small_number = 123;
    int32_t negative_number = -123456;

    clearScreen();
    setCursorPosition(0, 0);
    print_string("Testing stdint.h...");

    setCursorPosition(6, 25);
    
    if (number > small_number) {
        print_string("uint32_t test passed!");
    }

    bool_t isTrue = true;
    setCursorPosition(12, 50);
    if (isTrue) {
        print_string("Boolean test passed!");
    }

    while (1);  // Loop indefinitely
}
