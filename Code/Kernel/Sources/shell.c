#include "../Includes/shell.h"

extern TextStyle backGroundColor;
extern uint16_t keyboard_cursor_position;
extern float A , B, C;
bool_t cube_active = false;

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
    ParsedCommand parsedCommand = {0};
    parsedCommand.command = NULL;
    parsedCommand.arg_count = 0;

    char* command = strtok(input, " ");
    if (command == NULL) {
        return parsedCommand;
    }

    parsedCommand.command = command;

    if (strcmp(command, "echo") == 0) {
        char* content = strtok(NULL, ">");
        if (content != NULL) {
            parsedCommand.arguments[parsedCommand.arg_count++] = content;

            while (*content == ' ') content++;

            char* target_file = strtok(NULL, " ");
            if (target_file != NULL) {
                parsedCommand.arguments[parsedCommand.arg_count++] = target_file;
            } else {
                printf("Error: Invalid syntax. Usage: echo <content> > <file>\n", RED_ON_BLACK_WARNING);
                parsedCommand.command = "\0";
                return parsedCommand;
            }

            if (strtok(NULL, " ") != NULL) {
                printf("Error: Invalid syntax. Too many arguments for file name.\n", RED_ON_BLACK_WARNING);
                parsedCommand.command = "\0";
                return parsedCommand;
            }
        } else {
            printf("Error: Invalid syntax. Usage: echo <content> > <file>\n", RED_ON_BLACK_WARNING);
            parsedCommand.command = "\0";
            return parsedCommand;
        }
    } else {
        char* arg = strtok(NULL, " ");
        while (arg && parsedCommand.arg_count < 10) {
            parsedCommand.arguments[parsedCommand.arg_count++] = arg;
            arg = strtok(NULL, " ");
        }
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
            // Total memory is always full
            printf("I", COLOR_BLACK_ON_WHITE);
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
    } else if (arg_count > 0 && strcmp(arguments[0], "--kibi") == 0) {
        // Display memory usage in KiB
        scrollIfNeeded(row);
        printf("Total Memory: %u KiB\n", COLOR_BLACK_ON_WHITE, totalMemory / 1024);
        ++row;
        scrollIfNeeded(row);

        printf("Used Memory: %u KiB\n", COLOR_BLACK_ON_WHITE, usedMemory / 1024);
        ++row;
        scrollIfNeeded(row);

        printf("Free Memory: %u KiB\n", COLOR_BLACK_ON_WHITE, freeMemory / 1024);
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

void handleCubeCommand() {
    while (cube_active) {
        renderCube();
        A += 0.13;
        B += 0.06;
        C += 0.09;
        yield();
    }
    set_task_state(current, TERMINATED);
    clear();
    yield();
}

void handleHelpCommand(ParsedCommand parsedCommand) {
    if (parsedCommand.command && strcmp((const uint8_t*)parsedCommand.command, (const uint8_t*)"help") == 0) {
        showHelp();
        nextLine();
    }
}

void showHelp() {
    nextLine();
    uint16_t row = keyboard_cursor_position / VGA_COLS;

    const char* helpText[] = {
        "Shell Commands:",
        "clear                     - Clear the screen.",
        "echo <text> > <file>      - Write text to a file.",
        "ls                        - List all files.",
        "cat <file>                - Display the contents of a file.",
        "touch <file>              - Create a new empty file.",
        "rm <file>                 - Remove a file.",
        "fs --init                 - Initialize the file system. (NOT IN USED)",
        "free [--bar | --kibi]     - Show memory usage (bar graph or in KiB).",
        "cube [--on | --off]       - Enable or disable the rotating cube.",
        "help                      - Show this help message.",
        "test [--all | --some]     - Run memory tests or create fragmentation."
    };

    for (size_t i = 0; i < sizeof(helpText) / sizeof(helpText[0]); ++i) {
        scrollIfNeeded(row);
        printf("%s\n", COLOR_BLACK_ON_WHITE, helpText[i]);
        row++;
    }

    setCursorPosition(row, 0);
    keyboard_cursor_position = row * VGA_COLS;
}

void print_prompt() {
    printf(SHELL_PROMPT, COLOR_BLACK_ON_WHITE);

    keyboard_cursor_position += strlen(SHELL_PROMPT);

    uint16_t row = keyboard_cursor_position / VGA_COLS;
    uint16_t col = keyboard_cursor_position % VGA_COLS;
    setCursorPosition(row, col);
}

void init_shell() {
    uint16_t initial_row = 2;
    uint16_t initial_col = 0;
    keyboard_cursor_position = initial_row * VGA_COLS + initial_col;

    setCursorPosition(initial_row, initial_col);

    print_prompt();
}