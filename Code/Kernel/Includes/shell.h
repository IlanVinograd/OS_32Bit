#ifndef _SHELL_H_
#define _SHELL_H_

#include <vga.h>

void scrollIfNeeded(int row);
void handleBackgroundColor(const char* input);
void clear();

#endif