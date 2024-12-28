#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <io.h>
#include <vga.h>
#include <shell.h>
#include <ata.h>

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define INPUT_BUFFER_SIZE 256

void handle_keyboard_input();
int8_t scancode_to_ascii(uint8_t scancode);
void enable_keyboard();
void disable_keyboard();
int8_t apply_shift_to_key(int8_t key);
void blinkCursor(bool_t cursorSignal);
void handle_backspace();
void handle_enter();
void scroll_screen();

#endif /* _KEYBOARD_H_ */