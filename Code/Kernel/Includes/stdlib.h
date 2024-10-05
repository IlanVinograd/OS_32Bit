#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <stdint.h>
#include <string.h>

void itoa(int num, char* str, int base);
// Converts the integer 'num' into a string 'str' in the specified 'base' (e.g., 10 for decimal, 16 for hex).

int32_t atoi(const char* str);
// Converts the string 'str' into an integer (assumes the string represents a valid number).

int32_t boolToChar(bool_t vIn, uint8_t* str);
// Converts a boolean value to a string representation.

int32_t hexToString(uint64_t num, uint8_t* str);
// Converts a hexadecimal value to a string.

#endif /* _STDLIB_H_ */