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