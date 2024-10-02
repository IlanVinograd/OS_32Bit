#ifndef _VGA_H_
#define _VGA_H_

#include <stdint.h>
#include <../Includes/io.h>

#define VGA_VIDEO_MEMORY 0xB8000
#define VGA_ROWS 25
#define VGA_COLS 80
#define VGA_SHELL_BEGIN 0

void clearScreen(void);
void initScreen(void);
void printf(const char *str);
void setCursorPosition(uint16_t row, uint16_t col);
void getCursorPosition(uint16_t *row, uint16_t *col);
void setColor(uint8_t color);

#endif /* _VGA_H_ */