#include "../Includes/vga.h"

uint16_t CursorPosition;
TextStyle currentColor;
char* osVersion;

volatile uint16_t *const video_text_mem = (uint16_t *)VGA_VIDEO_MEMORY;

void initScreen(char* version) {
    osVersion = version;

    // Fill the first two rows with a blue background
    fillFirstTwoLinesBlue();

    // Set the cursor position to start below the blue rows
    setCursorPosition(2, 0);
}

void clearScreen(void) {
    uint8_t *video_address = (uint8_t *)video_text_mem;
    
    for (uint32_t row = 1; row < VGA_ROWS; row++) { // Skip row 0
        for (uint32_t col = 0; col < VGA_COLS; col++) {
            uint32_t index = (row * VGA_COLS + col) * 2; // Each character is 2 bytes
            video_address[index] = ' ';                 // Blank character
            video_address[index + 1] = 0x07;            // Default VGA attribute
        }
    }
}

void setCursorPosition(uint16_t row, uint16_t col) {
    uint16_t position = row * VGA_COLS + col;

    outPort(0x3D4, 0x0F);
    outPort(0x3D5, (uint8_t)(position & 0xFF));
    outPort(0x3D4, 0x0E);
    outPort(0x3D5, (uint8_t)((position >> 8) & 0xFF));

    CursorPosition = position;
}

uint16_t getCursorPosition(void) {
    uint16_t position = 0;

    outPort(0x3D4, 0x0F);
    uint8_t low_byte = inPort(0x3D5);

    outPort(0x3D4, 0x0E);
    uint8_t high_byte = inPort(0x3D5);

    position = ((uint16_t)high_byte << 8) | low_byte;

    return position;
}

uint8_t encodeColor(TextStyle style) {
    return (style.blinking_Foreground_Intensity << 7) |
           (style.backround << 4) |
           (style.Foreground_Intensity << 3) |
           (style.foreground);
}

void putc(char c, TextStyle style){
    uint16_t cursor_position = getCursorPosition();
    uint8_t color = encodeColor(style);
    video_text_mem[cursor_position] = (c & 0xFF) | (color << 8);

    cursor_position++;
    setCursorPosition(cursor_position / VGA_COLS, cursor_position % VGA_COLS);
}

void printf(const char* fmt, TextStyle style, ...) {
    va_list args;
    va_start(args, style);

    char c;
    while ((c = *fmt++) != '\0') {
        if (c == '%') {
            // Handle format specifiers
            char next = *fmt++;
            int width = 0; // Field width for padding
            bool_t zero_pad = false;

            // Parse width and padding
            if (next == '0') {
                zero_pad = true;
                next = *fmt++;
            }
            while (next >= '0' && next <= '9') {
                width = width * 10 + (next - '0');
                next = *fmt++;
            }

            switch (next) {
                case 'c': {
                    // Print a character
                    char ch = (char)va_arg(args, int);
                    putc(ch, style);
                    break;
                }
                case 's': {
                    // Print a string
                    char* str = va_arg(args, char*);
                    while (*str) {
                        putc(*str++, style);
                    }
                    break;
                }
                case 'd': {
                    // Print an integer with optional padding
                    int num = va_arg(args, int);
                    char buffer[16];  // Buffer to hold the string representation
                    itoa(num, buffer, 10);
                    int len = strlen((uint8_t*)buffer);

                    if (zero_pad) {
                        while (len < width) {
                            putc('0', style);
                            len++;
                        }
                    }
                    char* p = buffer;
                    while (*p) {
                        putc(*p++, style);
                    }
                    break;
                }
                case 'u': {
                    // Print an unsigned integer
                    unsigned int num = va_arg(args, unsigned int);
                    char buffer[16];
                    utoa(num, buffer, 10);
                    int len = strlen((uint8_t*)buffer);

                    if (zero_pad) {
                        while (len < width) {
                            putc('0', style);
                            len++;
                        }
                    }
                    char* p = buffer;
                    while (*p) {
                        putc(*p++, style);
                    }
                    break;
                }
                case 'x': {
                    // Print a hexadecimal integer
                    int num = va_arg(args, int);
                    char buffer[16];
                    itoa(num, buffer, 16);
                    char* p = buffer;
                    while (*p) {
                        putc(*p++, style);
                    }
                    break;
                }
                case 'p': {
                    uintptr_t addr = va_arg(args, uintptr_t);
                    putc('0', style);
                    putc('x', style);
                    char buffer[16];
                    utoa(addr, buffer, 16); // Convert address to hex
                    char* p = buffer;
                    while (*p) {
                        putc(*p++, style);
                    }
                    break;
                }
                case 'X': {  // Uppercase hexadecimal
                    uint32_t num = va_arg(args, uint32_t);
                    char buffer[16];
                    utoa(num, buffer, 16); // Convert to hexadecimal
                    char* p = buffer;
                    while (*p) {
                        putc((*p >= 'a' && *p <= 'f') ? *p - 32 : *p, style); // Convert to uppercase
                        p++;
                    }
                    break;
                }
                default: {
                    // Print unknown format as-is
                    putc('%', style);
                    putc(next, style);
                    break;
                }
            }
        } else if (c == '\n') {
            // Handle newline character
            uint16_t cursor_position = getCursorPosition();
            uint16_t row = cursor_position / VGA_COLS;
            row++;
            setCursorPosition(row, 0);
        } else {
            putc(c, style);
        }
    }

    va_end(args);
}

void scroll_screen() {
    uint16_t *video_address = (uint16_t *)video_text_mem;

    // Copy each row to the row above it, skipping the first two rows
    for (uint32_t row = 3; row < VGA_ROWS; row++) {
        for (uint32_t col = 0; col < VGA_COLS; col++) {
            video_address[(row - 1) * VGA_COLS + col] = video_address[row * VGA_COLS + col];
        }
    }

    // Clear the last row (bottom row)
    uint8_t color = encodeColor(COLOR_BLACK_ON_WHITE); // Default background color
    for (uint32_t col = 0; col < VGA_COLS; col++) {
        video_address[(VGA_ROWS - 1) * VGA_COLS + col] = (' ' & 0xFF) | (color << 8);
    }
}

void fillFirstTwoLinesBlue(void) {
    uint8_t *video_address = (uint8_t *)video_text_mem;

    for (uint32_t row = 0; row < 2; row++) { // Only the first two rows
        for (uint32_t col = 0; col < VGA_COLS; col++) {
            uint32_t index = (row * VGA_COLS + col) * 2; // Each character is 2 bytes
            video_address[index] = ' '; // Blank character
            video_address[index + 1] = encodeColor(COLOR_BLUE_ON_WHITE); // Blue background
        }
    }
}

