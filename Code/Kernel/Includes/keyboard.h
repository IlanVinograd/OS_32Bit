#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <io.h>
#include <vga.h>

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

void handle_keyboard_input();
int8_t scancode_to_ascii(uint8_t scancode);
void enable_keyboard();
int8_t apply_shift_to_key(int8_t key);

#endif /* _KEYBOARD_H_ */