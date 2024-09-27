#define VIDEO_MEMORY 0xB8000
#define WHITE_ON_BLACK 0x0F
#define ROWS 25
#define COLS 80

void clear_screen() {
    volatile unsigned short *video = (unsigned short *) VIDEO_MEMORY;
    for (int i = 0; i < ROWS * COLS; i++) {
        video[i] = ' ' | (WHITE_ON_BLACK << 8);
    }
}

void print_string(const char *message) {
    volatile unsigned short *video = (unsigned short *) VIDEO_MEMORY;
    while (*message != '\0') {
        *video++ = (*message++) | (WHITE_ON_BLACK << 8);
    }
}

void _start(void) {
    clear_screen();
    print_string("Hello, Kernel World! ;D");
    while(1);
}