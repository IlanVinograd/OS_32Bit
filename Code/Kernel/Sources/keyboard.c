#include "../Includes/keyboard.h"

uint16_t keyboard_cursor_position = VGA_COLS;
extern uint16_t CursorPosition;
bool_t shift_pressed = false;
extern bool_t cube_active;

char* inputBuffer;
uint32_t inputBufferIndex = 0;
uint32_t inputBufferSize = 256;

extern TextStyle backGroundColor;
extern SuperBlock SP;

void handle_keyboard_input() {
    uint8_t scancode = inPort(KEYBOARD_DATA_PORT);

    // Check for Shift press and release
    if (scancode == 0x2A || scancode == 0x36) { // Left Shift pressed
        shift_pressed = true;
        return;
    } else if (scancode == 0xAA || scancode == 0xB6) { // Left Shift released
        shift_pressed = false;
        return;
    }

    if (scancode == 0x0E) { // Backspace key
        handle_backspace();
        return;
    }

    if (scancode == 0x1C) { // Enter key
        handle_enter();
        return;
    }

    int8_t key = scancode_to_ascii(scancode);

    if (key != '\0') {
        if (shift_pressed) {
            key = apply_shift_to_key(key);
        }

        // Add the key to the buffer
        if (inputBufferIndex < inputBufferSize - 1) { // Prevent buffer overflow
            inputBuffer[inputBufferIndex++] = key;
            inputBuffer[inputBufferIndex] = '\0'; // Null-terminate the buffer
        } else {
            // Double the buffer size dynamically
            char* new_buffer = realloc_safe(inputBuffer, inputBufferSize * 2, inputBufferSize);
            if (new_buffer) {
                inputBuffer = new_buffer;
                inputBufferSize *= 2; // Update the buffer size
            } else {
                printf("Error: Could not expand input buffer.\n", RED_ON_BLACK_WARNING);
            }
        }

        // Display the key on the screen
        uint16_t row = keyboard_cursor_position / VGA_COLS;
        uint16_t col = keyboard_cursor_position % VGA_COLS;

        setCursorPosition(row, col);
        putc(key, COLOR_BLACK_ON_WHITE);

        // Move the keyboard cursor
        keyboard_cursor_position++;
        if (keyboard_cursor_position >= VGA_COLS * VGA_ROWS) {
            // Scroll the screen up by one row
            scroll_screen();
            keyboard_cursor_position -= VGA_COLS; // Adjust to the last row
        }
    }
    blinkCursor(true);
}

int8_t scancode_to_ascii(uint8_t scancode) {
    static char scancode_map[] = {
        '\0', '\x1B', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\0',
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '\\',
        'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '*', '\0', ' '
    };

    if (scancode < sizeof(scancode_map)) {
        return scancode_map[scancode];
    }
    return '\0';
}

int8_t apply_shift_to_key(int8_t key) {
    static const char shift_map[128] = {
        ['1'] = '!', ['2'] = '@', ['3'] = '#', ['4'] = '$', ['5'] = '%',
        ['6'] = '^', ['7'] = '&', ['8'] = '*', ['9'] = '(', ['0'] = ')',
        ['-'] = '_', ['='] = '+', ['['] = '{', [']'] = '}', [';'] = ':',
        ['\''] = '"', ['\\'] = '|', [','] = '<', ['.'] = '>', ['/'] = '?',
        ['`'] = '~', ['a'] = 'A', ['b'] = 'B', ['c'] = 'C', ['d'] = 'D',
        ['e'] = 'E', ['f'] = 'F', ['g'] = 'G', ['h'] = 'H', ['i'] = 'I',
        ['j'] = 'J', ['k'] = 'K', ['l'] = 'L', ['m'] = 'M', ['n'] = 'N',
        ['o'] = 'O', ['p'] = 'P', ['q'] = 'Q', ['r'] = 'R', ['s'] = 'S',
        ['t'] = 'T', ['u'] = 'U', ['v'] = 'V', ['w'] = 'W', ['x'] = 'X',
        ['y'] = 'Y', ['z'] = 'Z'
    };

    if (key >= 0 && key < 128 && shift_map[key]) {
        return shift_map[key];
    }
    return key; // Return original key if no mapping
}

void enable_keyboard() {
    inputBuffer = (char*)malloc(INPUT_BUFFER_SIZE); // Allocate buffer memory
    inputBuffer[0] = '\0'; // Initialize the buffer

    uint8_t mask = inPort(PIC1_DATA);  // Read current PIC1 interrupt mask
    mask &= ~(1 << 1);                 // Clear the bit for IRQ1 (keyboard)
    outPort(PIC1_DATA, mask);          // Write back the updated mask
}

void disable_keyboard() {
    if (inputBuffer) {
        free(inputBuffer); // Free the buffer memory
        inputBuffer = NULL;
    }
}

void blinkCursor(bool_t cursorSignal) {
    uint16_t row = keyboard_cursor_position / VGA_COLS;
    uint16_t col = keyboard_cursor_position % VGA_COLS;

    setCursorPosition(row, col); // Set cursor position

    if (cursorSignal) {
        putc('_', COLOR_BLACK_ON_WHITE_BLINKING); // Draw blinking cursor
    } else {
        putc(' ', COLOR_BLACK_ON_WHITE_BLINKING); // Clear cursor
    }

    setCursorPosition(row, col);
}

void handle_backspace() {
    // Ensure we're not at the first position (row 0, col 0)
    if (keyboard_cursor_position > VGA_COLS && inputBufferIndex > 0) {
        uint16_t row, col;

        // Clear the current blinking cursor at the current position
        row = keyboard_cursor_position / VGA_COLS;
        col = keyboard_cursor_position % VGA_COLS;
        setCursorPosition(row, col);
        putc(' ', COLOR_BLACK_ON_WHITE);

        // Move the cursor back one position
        keyboard_cursor_position--;
        inputBufferIndex--;

        // Clear the character at the new cursor position
        row = keyboard_cursor_position / VGA_COLS;
        col = keyboard_cursor_position % VGA_COLS;
        setCursorPosition(row, col);
        putc(' ', COLOR_BLACK_ON_WHITE);

        // Null-terminate the buffer at the new position
        inputBuffer[inputBufferIndex] = '\0';

        // Redraw the blinking cursor at the new position
        setCursorPosition(row, col);
        blinkCursor(true);
    }
}

void handle_enter() {
    uint16_t row = keyboard_cursor_position / VGA_COLS;
    uint16_t col = keyboard_cursor_position % VGA_COLS;

    // Clear the blinking cursor before processing
    setCursorPosition(row, col);
    putc(' ', COLOR_BLACK_ON_WHITE);
    
    // Shell Functions
    scrollIfNeeded(row);
    handleBackgroundColor(inputBuffer);

    // Parse input command
    ParsedCommand parsedCommand = parseCommand((char*)inputBuffer);

    // Move cursor to the next line
    row++;
    if (row >= VGA_ROWS) {
        // Scroll again if we're still at the last row
        scroll_screen();
        row = VGA_ROWS - 1;
    }
    keyboard_cursor_position = row * VGA_COLS;

    setCursorPosition(row, 0);

    if (parsedCommand.command && strcmp(parsedCommand.command, "test") == 0) {
        if (parsedCommand.arg_count > 0 && strcmp(parsedCommand.arguments[0], "--all") == 0) {
            // Allocate all memory
            test_full_allocation();
        } else if (parsedCommand.arg_count > 0 && strcmp(parsedCommand.arguments[0], "--some") == 0) {
            // Create fragmentation
            test_fragmentation();
        } else {
            create_task((uintptr_t)test);
        }
    }
    
    if (parsedCommand.command && strcmp(parsedCommand.command, "cube") == 0) {
        if (parsedCommand.arg_count > 0 && strcmp(parsedCommand.arguments[0], "--on") == 0) {
            if (!cube_active) {
                cube_active = true;
                create_task((uintptr_t)handleCubeCommand);
            } 
        } else if (parsedCommand.arg_count > 0 && strcmp(parsedCommand.arguments[0], "--off") == 0) {
            if (cube_active) {
                cube_active = false;
            }
        }
    }

    if (parsedCommand.command && strcmp(parsedCommand.command, "fs") == 0) { // Do this ONLY if needed.
        if (parsedCommand.arg_count > 0 && strcmp(parsedCommand.arguments[0], "--init") == 0) {
            init_fs();
        }
    }
    
    if (parsedCommand.command && strcmp(parsedCommand.command, "touch") == 0) {
        if (parsedCommand.arg_count == 1) {
            create_file(parsedCommand.arguments[0]);
            
        } else {
            printf("Usage: touch <filename>\n", backGroundColor);

            row += 1;
            if (row >= VGA_ROWS) {
                scroll_screen();
                row = VGA_ROWS - 1;
            }
            keyboard_cursor_position = row * VGA_COLS;
            setCursorPosition(row, 0);
        }
    }

/*
    if (parsedCommand.command && strcmp((const uint8_t*)parsedCommand.command, (const uint8_t*)"read") == 0) {
        printf("Start reading from slave drive!\n", RED_ON_BLACK_WARNING);

        uint8_t buffer[ATA_SECTOR_SIZE];
        memset(buffer, 0, ATA_SECTOR_SIZE);

        ata_identify(ATA_PRIMARY_IO, ATA_SLAVE);
        ata_read(ATA_PRIMARY_IO, ATA_SLAVE, 0, 1, buffer);

        printf("Data read (first 16 bytes):\n", RED_ON_BLACK_WARNING);
        for (int i = 0; i < 16; i++) {
            uint8_t value = buffer[i];
            if (value < 0x10) {
                printf("0x0%X ", COLOR_BLACK_ON_WHITE, value);
            } else {
                printf("0x%X ", COLOR_BLACK_ON_WHITE, value);
            }
        }
        printf("\nRead Complete!\n", RED_ON_BLACK_WARNING);
    }

    if (parsedCommand.command && strcmp((const uint8_t*)parsedCommand.command, (const uint8_t*)"write") == 0) {
        uint8_t buffer[ATA_SECTOR_SIZE];
        for (int i = 0; i < ATA_SECTOR_SIZE; i++) {
            buffer[i] = i % 256;
        }

        for (uint32_t lba = 0; lba <= 16; lba++) {
            ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
            ata_write(ATA_PRIMARY_IO, ATA_MASTER, lba, 1, buffer);
        }

        printf("Write operation completed!\n", RED_ON_BLACK_WARNING);
    }
*/
    // Recognize and process free command
    if(parsedCommand.command && strcmp(parsedCommand.command, "free") == 0) {
        handleFreeCommand(parsedCommand.arg_count, parsedCommand.arguments);
    }

    // Check 'clear' command
    if (parsedCommand.command && strcmp(parsedCommand.command, "clear") == 0) {
        clear();
        scroll_screen();
    }

    // Reset input buffer to default size
    free(inputBuffer); // Free the current buffer
    inputBufferSize = 256; // Reset the buffer size
    inputBuffer = (char*)malloc(inputBufferSize); // Allocate a new buffer
    if (!inputBuffer) {
        printf("Error: Could not reallocate input buffer.\n", RED_ON_BLACK_WARNING);
        return;
    }
    inputBuffer[0] = '\0'; // Initialize the buffer
    inputBufferIndex = 0;  // Reset the index
}