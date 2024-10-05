#include "../Includes/vga.h"

uint16_t CursorPosition;
TextStyle currentColor;

void initScreen(char* version) {
    printf("                 ,\n"
           "                /|      ___\n"
           "               / |   ,-~ /\n"
           "              Y :|  //  /\n"
           "              | jj /( .^\n"
           "              >-\"~\"-v\"\n"
           "             /       Y\n"
           "            jo  o    |\n"
           "           ( ~T~     j\n"
           "            >._-' _./\n"
           "           /   \"~\"  |\n"
           "          Y     _,  |\n"
           "         /| ;-\"~ _  l\n"
           "        / l/ ,-\"~    \\\n"
           "        \\//\\//      .- \\\n"
           "         Y        /    Y\n"
           "         l       I     !\n"
           "         ]\\      _\\    /\"\\\n"
           "        (\" ~----( ~   Y.  )\n"
           "    ~~~~~~~~~~~~~~~~~~~~~~~~~\n", COLOR_BLACK_ON_WHITE);
           
    setCursorPosition(8, 14);
    putc('T', RED_ON_BLACK_WARNING);
    setCursorPosition(20, 4);
    printf("Version OS: %s", COLOR_BLINKING_YELLOW, version);

    setCursorPosition(3, 33);
    printf("Shell Will be here",GREEN_ON_BLACK_SUCCESS);
    setCursorPosition(5, 33);
    
    //Continue Init Here
}

void clearScreen(void){
    uint8_t *video_address = (uint8_t *) VGA_VIDEO_MEMORY;
    uint32_t i = 0;

    while(i + VGA_SHELL_BEGIN < (VGA_ROWS * VGA_COLS) - VGA_SHELL_BEGIN){
        video_address[i] = ' ';
        i += 2;
    }
    CursorPosition = VGA_SHELL_BEGIN;
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
    volatile uint16_t *video = (uint16_t *)VGA_VIDEO_MEMORY + cursor_position;
    uint8_t color = encodeColor(style);
    *video = (c & 0xFF) | (color << 8);

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
            uint16_t col = cursor_position % VGA_COLS;

            // Move to the beginning of the next row
            row++;
            setCursorPosition(row, 0);
        } else {
            putc(c, style);
        }
    }

    va_end(args);
}