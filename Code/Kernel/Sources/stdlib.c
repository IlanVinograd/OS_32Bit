#include "../Includes/stdlib.h"

// Helper function for integer to string conversion (signed integers)
void itoa(int num, char* str, int base) {
    int i = 0;
    bool_t isNegative = false;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }

    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// Helper function for unsigned integer to string conversion
void utoa(uint32_t num, char* str, int32_t base) {
    int i = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    str[i] = '\0';

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

int32_t atoi(const char* str) {
    int32_t result = 0;
    int32_t sign = 1;

    while (*str == ' ' || *str == '\t' || *str == '\n') {
        str++;
    }

    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return result * sign;
}

void boolToChar(uint8_t value, uint8_t *str) {
    if (value) {
        strcpy(str, (uint8_t *)"true");
    } else {
        strcpy(str, (uint8_t *)"false");
    }
}

int32_t hexToString(uint64_t num, uint8_t* str) {
    const char hexChars[] = "0123456789ABCDEF";
    int32_t i = 0;
    int32_t started = 0;

    for (int32_t shift = 60; shift >= 0; shift -= 4) {
        uint8_t nibble = (num >> shift) & 0xF;
        if (nibble != 0 || started) {
            str[i++] = hexChars[nibble];
            started = 1;
        }
    }

    if (i == 0) str[i++] = '0';

    str[i] = '\0';

    return i;
}