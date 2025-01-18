#ifndef _SHELL_H_
#define _SHELL_H_

#include <vga.h>
#include <render.h>
#include <test.h>

typedef struct {
    char* command; // command
    char* arguments[10]; // parameters
    int arg_count; // the number of parameters
} ParsedCommand;

void scrollIfNeeded(int row);
void handleBackgroundColor(const char* input);
void clear();
ParsedCommand parseCommand(char* input);
void handleFreeCommand(int arg_count, char* arguments[]);
void handleCubeCommand();
void handleHelpCommand();
void handleTestCommand(ParsedCommand parsedCommand);

void showHelp();
void print_prompt();
void init_shell();
void startCube();

#endif /* _SHELL_H_ */