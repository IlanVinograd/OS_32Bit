#include <string.h>

uint8_t *strcpy(uint8_t *dest, const uint8_t *src){
    uint8_t *original_dest = dest;

    while(*src != '\0'){
        *dest = *src;
        src++;
        dest++;
    }

    *dest = '\0';
    return original_dest;
}

uint8_t *strncpy(uint8_t *dest, const uint8_t *src, size_t n){
    uint8_t *original_dest = dest;
    size_t count = 0;

    while(count < n && *src != '\0'){
        *dest = *src;
        src++;
        dest++;
        count++;
    }

    while(count < n) {
        *dest = '\0';
        dest++;
        count++;
    }

    return original_dest;
}

uint8_t *strcat(uint8_t *dest, const uint8_t *src){
    uint8_t *original_dest = dest;

    while(*dest != '\0') dest++;

    while(*src != '\0'){
        *dest = *src;
        dest++;
        src++;
    }

    *dest = '\0';
    return original_dest;
}

uint8_t *strncat(uint8_t *dest, const uint8_t *src, size_t n){
    uint8_t *original_dest = dest;
    size_t count = 0;

    while(*dest != '\0') dest++;

    while(*src != '\0' && count < n){
        *dest = *src;
        dest++;
        src++;
        count++;
    }

    *dest = '\0';
    return original_dest;
}

size_t strlen(const uint8_t *str){
    size_t count = 0;

    while(*str != '\0') {
        count++;
        str++;
    }

    return count;
}

int32_t strcmp(const uint8_t *str1, const uint8_t *str2) {
    while (*str1 != '\0' && *str2 != '\0') {
        if (*str1 != *str2) return *str1 - *str2;

        str1++;
        str2++;
    }

    return *str1 - *str2;
}

int32_t strncmp(const uint8_t *str1, const uint8_t *str2, size_t n) {
    size_t count = 0;

    while (count < n) {

        if (*str1 != *str2) return *str1 - *str2;

        if (*str1 == '\0' || *str2 == '\0') break;
    
        str1++;
        str2++;
        count++;
    }

    return NULL;
}

uint8_t *strchr(const uint8_t *str, int32_t c){
    while(*str != '\0'){
        if(*str == (uint8_t)c) return (uint8_t *)str;

        str++;
    }

    if (c == '\0') return (uint8_t *)str;

    return NULL;
}

uint8_t *strrchr(const uint8_t *str, int32_t c){
    uint8_t *last_occur = 0;

    while(*str != '\0'){
        if(*str == (uint8_t)c) last_occur = (uint8_t *)str;

        str++;
    }

    if (c == '\0') return (uint8_t *)str;

    return last_occur;
}

uint8_t *strstr(const uint8_t *haystack, const uint8_t *needle){
    if (*needle == '\0') return (uint8_t *)haystack;

    while (*haystack != '\0') {
        const uint8_t *h = haystack;
        const uint8_t *n = needle;

        while (*h == *n && *n != '\0') {
            h++;
            n++;
        }

        if (*n == '\0') return (uint8_t *)haystack;

        haystack++;
    }

    return NULL;
}

uint8_t *strtok(uint8_t *str, const uint8_t *delim){
    static uint8_t *saved_str = NULL;
    uint8_t *token_start;

    if (str == NULL) str = saved_str;

    if (str == NULL) return NULL;

    while (*str != '\0' && strchr(delim, *str) != NULL) str++;

    if (*str == '\0') {
        saved_str = NULL;
        return NULL;
    }

    token_start = str;

    while (*str != '\0' && strchr(delim, *str) == NULL) str++;

    if (*str != '\0') {
        *str = '\0';
        str++;
    }

    saved_str = str;

    return token_start;
}

size_t strspn(const uint8_t *str1, const uint8_t *str2) {
    const uint8_t *s1 = str1;
    const uint8_t *s2;
    
    while (*s1 != '\0') {
        s2 = str2;
        while (*s2 != '\0') {
            if (*s1 == *s2) break;

            s2++;
        }

        if (*s2 == '\0') break;

        s1++;
    }

    return s1 - str1;
}

size_t strcspn(const uint8_t *str1, const uint8_t *str2) {
    const uint8_t *s1 = str1;
    const uint8_t *s2;

    while (*s1 != '\0') {
        s2 = str2;
        while (*s2 != '\0') {
            if (*s1 == *s2) return s1 - str1;

            s2++;
        }
        s1++;
    }

    return s1 - str1;
}

uint8_t *strpbrk(const uint8_t *str1, const uint8_t *str2) {

    while (*str1 != '\0') {

        const uint8_t *s2 = str2;
        while (*s2 != '\0') {
            if (*str1 == *s2) return (uint8_t *)str1;

            s2++;
        }
        str1++;
    }

    return NULL;
}

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    for (size_t i = 0; i < n; i++) d[i] = s[i];

    return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    if (s < d && d < s + n) {
        d += n;
        s += n;
        while (n--) *(--d) = *(--s);

    } else {
        for (size_t i = 0; i < n; i++) d[i] = s[i];
    }

    return dest;
}

int32_t memcmp(const void *str1, const void *str2, size_t n) {
    const uint8_t *s1 = (const uint8_t *)str1;
    const uint8_t *s2 = (const uint8_t *)str2;

    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) return s1[i] - s2[i];
    }

    return NULL;
}

void *memchr(const void *str, int32_t c, size_t n) {
    const uint8_t *s = (const uint8_t *)str;

    for (size_t i = 0; i < n; i++) {
        if (s[i] == (uint8_t)c) return (void *)(s + i);
    }

    return NULL;
}

void *memset(void *str, int32_t c, size_t n) {
    uint8_t *s = (uint8_t *)str;

    for (size_t i = 0; i < n; i++) {
        s[i] = (uint8_t)c;
    }

    return str;
}

void swap(uint8_t* p1, uint8_t* p2) {
    uint8_t temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

void reverse(uint8_t str[], int32_t length) {
    int32_t start = 0;
    int32_t end = length - 1;

    while (start < end) {
        uint8_t temp = str[start];
        str[start] = str[end];
        str[end] = temp;

        start++;
        end--;
    }
}

int32_t strCount(uint8_t* str, uint8_t c) {
    int32_t count = 0;

    while (*str != '\0') {
        if (*str == c) count++;

        str++;
    }

    return count;
}

uint8_t* deleteSpaces(uint8_t* str) {
    uint8_t* src = str;
    uint8_t* dest = str;

    // Traverse the string
    while (*src != '\0') {
        if (*src != ' ') {
            *dest = *src;
            dest++;
        }
        src++;
    }

    *dest = '\0';

    return str;
}

int32_t isNumber(uint8_t* num) {
    if (*num == '\0') return NULL;

    while (*num != '\0') {
        if (*num < '0' || *num > '9') return NULL;

        num++;
    }

    return 1;
}

