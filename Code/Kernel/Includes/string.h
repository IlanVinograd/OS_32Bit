#ifndef _STRING_H_
#define _STRING_H_

#include <stdint.h>

uint8_t *strcpy(uint8_t *dest, const uint8_t *src); 
// Copies 'src' to 'dest' (including the null terminator).

uint8_t *strncpy(uint8_t *dest, const uint8_t *src, size_t n);
// Copies up to 'n' characters from 'src' to 'dest'.

uint8_t *strcat(uint8_t *dest, const uint8_t *src);
// Appends 'src' to the end of 'dest'. abc + de = abcde

uint8_t *strncat(uint8_t *dest, const uint8_t *src, size_t n);
// Appends up to 'n' characters from 'src' to 'dest'.

size_t strlen(const char *str);
// Returns the length of the string 'str'.

int32_t strcmp(const uint8_t *str1, const uint8_t *str2);
// Compares 'str1' and 'str2'.

int32_t strncmp(const uint8_t *str1, const uint8_t *str2, size_t n);
// Compares up to 'n' characters from 'str1' and 'str2'.

uint8_t *strchr(const uint8_t *str, int32_t c);
// Returns a pointer to the first occurrence of 'c' in 'str'.

uint8_t *strrchr(const uint8_t *str, int32_t c);
// Returns a pointer to the last occurrence of 'c' in 'str'.

uint8_t *strstr(const uint8_t *haystack, const uint8_t *needle);
// Finds the first occurrence of 'needle' in 'haystack'.

uint8_t *strtok(uint8_t *str, const uint8_t *delim);
// Splits 'str' into tokens based on 'delim'. 

size_t strspn(const uint8_t *str1, const uint8_t *str2);
// Returns the length of the initial segment of 'str1' containing only characters from 'str2'.

size_t strcspn(const uint8_t *str1, const uint8_t *str2);
// Returns the length of the initial segment of 'str1' containing no characters from 'str2'.

uint8_t *strpbrk(const uint8_t *str1, const uint8_t *str2);
// Finds the first occurrence of any character from 'str2' in 'str1'.

void *memcpy(void *dest, const void *src, size_t n);
// Copies 'n' bytes from 'src' to 'dest'.

void *memmove(void *dest, const void *src, size_t n);
// Moves 'n' bytes from 'src' to 'dest' (handles overlap safely).

int32_t memcmp(const void *str1, const void *str2, size_t n);
// Compares the first 'n' bytes of 'str1' and 'str2'.

void *memchr(const void *str, int32_t c, size_t n);
// Finds the first occurrence of 'c' in the first 'n' bytes of 'str'.

void *memset(void *str, int32_t c, size_t n);
// Fills the first 'n' bytes of 'str' with the value 'c'.

void swap(uint8_t* p1, uint8_t* p2);
// Swaps two characters.

void reverse(uint8_t str[], int32_t length);
// Reverses the string.

int32_t strCount(uint8_t* str, uint8_t c);
// Counts how many times character 'c' appears in the string.

uint8_t* deleteSpaces(uint8_t* str);
// Removes spaces from the beginning and end of the string.

int32_t isNumber(uint8_t* num);
// Checks if the string represents a number.

#endif /* _STRING_H_ */