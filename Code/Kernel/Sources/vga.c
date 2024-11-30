#include "../Includes/vga.h"

uint16_t CursorPosition;
TextStyle currentColor;
char* osVersion;

volatile uint16_t *const video_text_mem = (uint16_t *)VGA_VIDEO_MEMORY;

void initScreen(char* version) {
    osVersion = version;
    //Continue Init Here
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
                    // Print an integer
                    int num = va_arg(args, int);
                    char buffer[16];  // Buffer to hold the string representation of the number
                    itoa(num, buffer, 10);  // Convert integer to string (base 10)
                    char* p = buffer;
                    while (*p) {
                        putc(*p++, style);
                    }
                    break;
                }
                case 'u': {
                    // Print an unsigned integer
                    unsigned int num = va_arg(args, unsigned int);
                    char buffer[16];  // Buffer to hold the string representation of the number
                    utoa(num, buffer, 10);  // Convert unsigned integer to string (base 10)
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
                    itoa(num, buffer, 16);  // Convert integer to hex string
                    char* p = buffer;
                    while (*p) {
                        putc(*p++, style);
                    }
                    break;
                }
                case 'p': {
                    // Print a pointer address
                    uintptr_t ptr = (uintptr_t)va_arg(args, void*);  // Cast the pointer to uintptr_t
                    char buffer[17];  // Buffer for the hexadecimal string
                    hexToString(ptr, (uint8_t*)buffer);  // Convert the address to a hexadecimal string
                    char* p = buffer;
                    while (*p) {
                        putc(*p++, style);
                    }
                    break;
                }
                case 'l': {
                    // Detect %llx for 64-bit hexadecimal
                    if (*fmt == 'l' && *(fmt + 1) == 'x') {
                        fmt += 2;  // Skip the 'llx'
                        uint64_t num = va_arg(args, uint64_t);
                        char buffer[17];  // Buffer for 64-bit hex string
                        hexToString(num, (uint8_t*)buffer);  // Convert 64-bit integer to hex string
                        char* p = buffer;
                        while (*p) {
                            putc(*p++, style);
                        }
                    }
                    break;
                }
                default: {
                    // If it's an unknown specifier, just print it as is
                    putc('%', style);
                    putc(next, style);
                    break;
                }
            }
        } else if (c == '\n') {
            // Handle newline character
            uint16_t cursor_position = getCursorPosition();
            uint16_t row = cursor_position / VGA_COLS;

            // Move to the beginning of the next row
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

    // Copy each row to the row above it
    for (uint32_t row = 1; row < VGA_ROWS; row++) {
        for (uint32_t col = 0; col < VGA_COLS; col++) {
            video_address[(row - 1) * VGA_COLS + col] = video_address[row * VGA_COLS + col];
        }
    }

    // Clear the last row
    uint8_t color = encodeColor(COLOR_BLACK_ON_WHITE);
    for (uint32_t col = 0; col < VGA_COLS; col++) {
        video_address[(VGA_ROWS - 1) * VGA_COLS + col] = (' ' & 0xFF) | (color << 8);
    }
}