#ifndef _SHELL_H_
#define _SHELL_H_

#include <vga.h>
#include <render.h>
#include <test.h>
#include <file_system.h>

typedef struct {
    char* command; // command
    char* arguments[10]; // parameters
    int arg_count; // the number of parameters
} ParsedCommand;

void scrollIfNeeded(int row);
void handleBackgroundColor(const char* input);
void clear();

ParsedCommand parseCommand(char* input);

void handleFreeCommand(ParsedCommand parsedCommand);
void handleTestCommand(ParsedCommand parsedCommand);
void handleLsCommand(ParsedCommand parsedCommand);
void handleClearCommand(ParsedCommand parsedCommand);
void handleFsCommand(ParsedCommand parsedCommand);
void handleCatCommand(ParsedCommand parsedCommand, uint16_t *row);
void handleTouchCommand(ParsedCommand parsedCommand, uint16_t *row);
void handleRmCommand(ParsedCommand parsedCommand, uint16_t *row);
void handleEchoCommand(ParsedCommand parsedCommand, uint16_t *row);
void handleCubeCommand();
void handleHelpCommand();

void showHelp();
void print_prompt();
void init_shell();
void startCube();
void print_memory(ParsedCommand parsedCommand);

#endif /* _SHELL_H_ */