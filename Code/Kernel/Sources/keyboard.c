#include "../Includes/keyboard.h"

uint16_t keyboard_cursor_position = VGA_COLS;
extern uint16_t CursorPosition;

void handle_keyboard_input(){
    uint8_t scancode = inPort(KEYBOARD_DATA_PORT);
    int8_t key = scancode_to_ascii(scancode);

    if (key != '\0') {
        uint16_t row = keyboard_cursor_position / VGA_COLS;
        uint16_t col = keyboard_cursor_position % VGA_COLS;

        // Print the character at the current keyboard cursor position
        setCursorPosition(row, col);
        putc(key, COLOR_BLACK_ON_WHITE);

        // Move the keyboard cursor to the next position
        keyboard_cursor_position++;
        if (keyboard_cursor_position >= VGA_COLS * VGA_ROWS) {
            keyboard_cursor_position = VGA_COLS; // Wrap to row 1
        }
    }
}

int8_t scancode_to_ascii(uint8_t scancode) {
    static char scancode_map[] = {
        '\0', '\x1B', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\0',
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '\\',
        'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '*', '\0', ' '
    };

    // Handle only known scancodes
    if (scancode < sizeof(scancode_map)) {
        return scancode_map[scancode];
    }
    return '\0';
}

void enable_keyboard(){
    uint8_t mask = inPort(PIC1_DATA);  // Read current PIC1 interrupt mask
    mask &= ~(1 << 1);                 // Clear the bit for IRQ1 (keyboard)
    outPort(PIC1_DATA, mask);          // Write back the updated mask
}