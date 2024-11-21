#ifndef _STDARG_H_
#define _STDARG_H_

typedef char* va_list;

// This macro calculates the size of an argument based on its type
#define __va_size(type) ((sizeof(type) + sizeof(int) - 1) & ~(sizeof(int) - 1))

// Initializes 'ap' to point to the first argument after 'last'
#define va_start(ap, last) (ap = (va_list)&last + __va_size(last))

// Retrieves the next argument in the list
#define va_arg(ap, type) (*(type *)((ap += __va_size(type)) - __va_size(type)))

// Cleans up the list (not needed in most cases)
#define va_end(ap) (ap = (va_list)0)

#endif /* _STDARG_H_ */