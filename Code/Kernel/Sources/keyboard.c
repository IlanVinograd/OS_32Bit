#include "../Includes/keyboard.h"

uint16_t keyboard_cursor_position = VGA_COLS;
extern uint16_t CursorPosition;
bool_t shift_pressed = false;

void handle_keyboard_input() {
    uint8_t scancode = inPort(KEYBOARD_DATA_PORT);

    // Check for Shift press and release
    if (scancode == 0x2A || scancode == 0x36) { // Left Shift (0x2A) or Right Shift (0x36) pressed
        shift_pressed = true;
        return;
    } else if (scancode == 0xAA || scancode == 0xB6) { // Left Shift (0xAA) or Right Shift (0xB6) released
        shift_pressed = false;
        return;
    }

    int8_t key = scancode_to_ascii(scancode);

    if (key != '\0') {
        if (shift_pressed) {
            key = apply_shift_to_key(key);
        }

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

    if (scancode < sizeof(scancode_map)) {
        return scancode_map[scancode];
    }
    return '\0';
}

int8_t apply_shift_to_key(int8_t key) {
    static const char shift_map[128] = {
        ['1'] = '!', ['2'] = '@', ['3'] = '#', ['4'] = '$', ['5'] = '%',
        ['6'] = '^', ['7'] = '&', ['8'] = '*', ['9'] = '(', ['0'] = ')',
        ['-'] = '_', ['='] = '+', ['['] = '{', [']'] = '}', [';'] = ':',
        ['\''] = '"', ['\\'] = '|', [','] = '<', ['.'] = '>', ['/'] = '?',
        ['`'] = '~', ['a'] = 'A', ['b'] = 'B', ['c'] = 'C', ['d'] = 'D',
        ['e'] = 'E', ['f'] = 'F', ['g'] = 'G', ['h'] = 'H', ['i'] = 'I',
        ['j'] = 'J', ['k'] = 'K', ['l'] = 'L', ['m'] = 'M', ['n'] = 'N',
        ['o'] = 'O', ['p'] = 'P', ['q'] = 'Q', ['r'] = 'R', ['s'] = 'S',
        ['t'] = 'T', ['u'] = 'U', ['v'] = 'V', ['w'] = 'W', ['x'] = 'X',
        ['y'] = 'Y', ['z'] = 'Z'
    };

    if (key >= 0 && key < 128 && shift_map[key]) {
        return shift_map[key];
    }
    return key; // Return original key if no mapping
}

void enable_keyboard() {
    uint8_t mask = inPort(PIC1_DATA);  // Read current PIC1 interrupt mask
    mask &= ~(1 << 1);                 // Clear the bit for IRQ1 (keyboard)
    outPort(PIC1_DATA, mask);          // Write back the updated mask
}