#include "../Includes/shell.h"

extern TextStyle backGroundColor;

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
        if (strcmp(input, "clear") == 0) {
            clearScreen();
            setCursorPosition(2, 0);
            return;
        }
        backGroundColor = mapInputToColor(input);
    }
}
