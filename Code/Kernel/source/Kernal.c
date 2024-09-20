#define VIDEO_MEMORY 0xB8000
#define ROWS 25
#define COLS 80
#define WHITE_ON_BLACK 0x0F

void print_string(const char *message) {
    volatile unsigned short *video = (unsigned short *) VIDEO_MEMORY;

    while (*message != '\0') {
        *video++ = (*message++) | (WHITE_ON_BLACK << 8);
    }
}

void kernel_main() {
    print_string("Hello World!");
}