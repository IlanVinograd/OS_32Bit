#include "../Includes/vga.h"

uint16_t CursorPosition;

void initScreen(void){

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

//to be deleted only for testing!!!!
void print_string(const char *message) {
    uint16_t cursor_position = getCursorPosition();
    volatile uint16_t *video = (uint16_t *)VGA_VIDEO_MEMORY + cursor_position;
    while (*message != '\0') {
        *video++ = (*message++) | (0x0F << 8);
        cursor_position++;
    }
    CursorPosition = cursor_position;
}