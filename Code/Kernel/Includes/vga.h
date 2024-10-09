#ifndef _VGA_H_
#define _VGA_H_

#include <stdint.h>
#include <io.h>
#include <stdlib.h>
#include <stdarg.h>
#include <kernel.h>

#define VGA_VIDEO_MEMORY (KERNEL_HIGH_BASE + 0xB8000)
#define VGA_ROWS 25
#define VGA_COLS 80
#define VGA_SHELL_BEGIN 0

#define COLOR_BLACK_ON_WHITE          (TextStyle){ White, 1, Black, 0 }
#define COLOR_GREEN_ON_BLUE           (TextStyle){ Blue, 1, Green, 0 }
#define COLOR_BLINKING_YELLOW         (TextStyle){ Light_Brown, 1, Black, 0 } // Blinking yellow text
#define COLOR_GRAY_ON_CYAN            (TextStyle){ Cyan, 0, Gray, 0 }
#define COLOR_LIGHT_RED_ON_DARK_GRAY  (TextStyle){ Dark_Gray, 1, Light_Red, 0 }

#define RED_ON_BLACK_WARNING          (TextStyle){ Red, 1, Black, 0 }
#define YELLOW_ON_BLACK_CAUTION       (TextStyle){ Light_Brown, 1, Black, 0 }
#define GREEN_ON_BLACK_SUCCESS        (TextStyle){ Green, 1, Black, 0 }



typedef struct {
    uint8_t foreground;
    uint8_t Foreground_Intensity;
    uint8_t backround;
    uint8_t blinking_Foreground_Intensity;
} __attribute__((packed)) TextStyle;

enum colors {
    Black, Blue, Green, Cyan, Red, Magenta, Brown, Gray, Dark_Gray,
    Light_Blue, Light_Green, Light_Cyan, Light_Red, Light_Magenta, Light_Brown, White,
};

void initScreen(char* version);
void clearScreen(void);
void setCursorPosition(uint16_t row, uint16_t col);
uint16_t getCursorPosition(void);
uint8_t encodeColor(TextStyle style);
void putc(char c, TextStyle style);
void printf(const char* fmt, TextStyle style, ...);

#endif /* _VGA_H_ */
