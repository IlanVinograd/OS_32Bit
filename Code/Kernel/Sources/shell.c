#include "../Includes/shell.h"

extern TextStyle backGroundColor;
extern uint16_t keyboard_cursor_position;

void scrollIfNeeded(int row) {
    if (row >= VGA_ROWS) {
        // Scroll the screen up by one row
        scroll_screen();
        row = VGA_ROWS - 1; // Keep the cursor on the last row
    }
    setCursorPosition(row, 0);
    keyboard_cursor_position = row * VGA_COLS;
}

void handleBackgroundColor(const char* input) {
    if (input && input[0] != '\0') {
        backGroundColor = mapInputToColor(input);
    }
}

void clear() {
    clearScreen();
    setCursorPosition(2, 0);
    keyboard_cursor_position = 2 * VGA_COLS;
}

ParsedCommand parseCommand(char* input) {
    ParsedCommand parsedCommand;
    parsedCommand.command = strtok(input, " ");
    parsedCommand.arg_count = 0;

    char* arg = strtok(NULL, " ");
    while (arg && parsedCommand.arg_count < 10) {
        parsedCommand.arguments[parsedCommand.arg_count++] = arg;
        arg = strtok(NULL, " ");
    }

    return parsedCommand;
}

void handleFreeCommand(int arg_count, char* arguments[]) {
    uint32_t totalMemory = NUM_PAGES * PAGE_SIZE;
    uint32_t usedMemory = pagesAllocated * PAGE_SIZE;
    uint32_t freeMemory = totalMemory - usedMemory;
    
    uint16_t row = keyboard_cursor_position / VGA_COLS;

    if (arg_count > 0 && strcmp(arguments[0], "--bar") == 0) {
        // Display memory usage as a bar
        scrollIfNeeded(row);
        printf("Total Memory  |", COLOR_BLACK_ON_WHITE);
        for (uint32_t i = 0; i < 20; i++) {
            if (i < (usedMemory * 20 / totalMemory)) {
                printf("I", COLOR_BLACK_ON_WHITE);
            } else {
                printf("-", COLOR_BLACK_ON_WHITE);
            }
        }
        printf("|\n", COLOR_BLACK_ON_WHITE);
        ++row;
        scrollIfNeeded(row);

        printf("Used Memory   |", COLOR_BLACK_ON_WHITE);
        for (uint32_t i = 0; i < 20; i++) {
            if (i < (usedMemory * 20 / totalMemory)) {
                printf("I", COLOR_BLACK_ON_WHITE);
            } else {
                printf("-", COLOR_BLACK_ON_WHITE);
            }
        }
        printf("|\n", COLOR_BLACK_ON_WHITE);
        ++row;
        scrollIfNeeded(row);

        printf("Free Memory   |", COLOR_BLACK_ON_WHITE);
        for (uint32_t i = 0; i < 20; i++) {
            if (i < (freeMemory * 20 / totalMemory)) {
                printf("I", COLOR_BLACK_ON_WHITE);
            } else {
                printf("-", COLOR_BLACK_ON_WHITE);
            }
        }
        printf("|\n", COLOR_BLACK_ON_WHITE);
        ++row;
        scrollIfNeeded(row);
    } else {
        // Display memory usage in MB
        scrollIfNeeded(row);
        printf("Total Memory: %u MB\n", COLOR_BLACK_ON_WHITE, totalMemory / (1024 * 1024));
        ++row;
        scrollIfNeeded(row);

        printf("Used Memory: %u MB\n", COLOR_BLACK_ON_WHITE, usedMemory / (1024 * 1024));
        ++row;
        scrollIfNeeded(row);

        printf("Free Memory: %u MB\n", COLOR_BLACK_ON_WHITE, freeMemory / (1024 * 1024));
        ++row;
        scrollIfNeeded(row);
    }

    // Move the cursor down three lines
    setCursorPosition(row, 0);
}