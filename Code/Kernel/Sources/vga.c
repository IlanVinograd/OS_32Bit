#include "../Includes/vga.h"

unsigned short CursorPosition;

void initScreen(void){

}

void clearScreen(void){
    uint8_t *video_address = (uint8_t *) VGA_VIDEO_MEMORY;
    uint32_t i = 0;

    while(i + VGA_SHELL_BEGIN < (VGA_ROWS * VGA_COLS) - VGA_SHELL_BEGIN){
        video_address[i] = ' ';
        i += 2;
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