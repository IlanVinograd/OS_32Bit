#ifndef _STDINT_H_
#define _STDINT_H_

/* Exact-width integer types */

/* 8-bit unsigned and signed integers */
typedef unsigned char  uint8_t;
typedef signed char    int8_t;

/* 16-bit unsigned and signed integers */
typedef unsigned short uint16_t;
typedef signed short   int16_t;

/* 32-bit unsigned and signed integers */
typedef unsigned int   uint32_t;
typedef signed int     int32_t;

/* 64-bit unsigned and signed integers */
typedef unsigned long long uint64_t;
typedef signed long long   int64_t;

/* Define pointer-sized integer types, useful for 32-bit systems */
typedef uint32_t uintptr_t;
typedef int32_t  intptr_t;

/* Define boolean type */
typedef uint8_t bool_t;    /* Typically a single byte for boolean values */
#define true  1          /* Define true as 1 */
#define false 0          /* Define false as 0 */

#define NULL 0

/* Define size_t for 32-bit system */
typedef unsigned int size_t;

#endif /* _STDINT_H_ */