#include "../Includes/ctype.h"

#include "ctype.h"

// Define the _ctype array. Each character has a corresponding set of flags.
char _ctype[] = {
    /* 0x00 - 0x07: Control characters */
    CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL,
    
    /* 0x08 - 0x0F: Control characters */
    CT_CTL, CT_WHT | CT_CTL, CT_WHT | CT_CTL, CT_WHT | CT_CTL, CT_WHT | CT_CTL, CT_WHT | CT_CTL, CT_CTL, CT_CTL,
    
    /* 0x10 - 0x17: Control characters */
    CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL,
    
    /* 0x18 - 0x1F: Control characters */
    CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL,
    
    /* 0x20: Space (hard space) */
    CT_WHT | CT_SP, 
    
    /* 0x21 - 0x2F: Punctuation */
    CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN,
    CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN,
    
    /* 0x30 - 0x39: Digits 0-9 */
    CT_DIG | CT_HEX, CT_DIG | CT_HEX, CT_DIG | CT_HEX, CT_DIG | CT_HEX,
    CT_DIG | CT_HEX, CT_DIG | CT_HEX, CT_DIG | CT_HEX, CT_DIG | CT_HEX,
    CT_DIG | CT_HEX, CT_DIG | CT_HEX,
    
    /* 0x3A - 0x40: Punctuation */
    CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN,
    
    /* 0x41 - 0x46: Uppercase A-F (hex digits) */
    CT_UP | CT_HEX, CT_UP | CT_HEX, CT_UP | CT_HEX, CT_UP | CT_HEX, CT_UP | CT_HEX, CT_UP | CT_HEX,
    
    /* 0x47 - 0x5A: Uppercase G-Z */
    CT_UP, CT_UP, CT_UP, CT_UP, CT_UP, CT_UP, CT_UP, CT_UP, CT_UP, CT_UP,
    CT_UP, CT_UP, CT_UP, CT_UP, CT_UP, CT_UP,
    
    /* 0x5B - 0x60: Punctuation */
    CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN,
    
    /* 0x61 - 0x66: Lowercase a-f (hex digits) */
    CT_LOW | CT_HEX, CT_LOW | CT_HEX, CT_LOW | CT_HEX, CT_LOW | CT_HEX, CT_LOW | CT_HEX, CT_LOW | CT_HEX,
    
    /* 0x67 - 0x7A: Lowercase g-z */
    CT_LOW, CT_LOW, CT_LOW, CT_LOW, CT_LOW, CT_LOW, CT_LOW, CT_LOW, CT_LOW, CT_LOW,
    CT_LOW, CT_LOW, CT_LOW, CT_LOW, CT_LOW, CT_LOW,
    
    /* 0x7B - 0x7F: Punctuation and control */
    CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_CTL
};