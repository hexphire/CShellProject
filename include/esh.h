#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

//Max size definitions, based on default Linux max.
#define MAX_PATH 4096
#define MAX_FILENAME 255
#define MAX_ARGS 32767

typedef struct cmd {
    char location[MAX_PATH]; //path to binary to execute
    char command[MAX_FILENAME]; //command or program to execute
    char fullCommand[MAX_PATH];
    char* args[MAX_ARGS]; //array of arguements passed along with command
    bool backgPro; //boolean flag for if command/proccess should be run in the background.
}cmd;

void eshPrompt();
void eshGetInput(struct cmd *curCommand);
void eshFindCmd(struct cmd *curCommand);
void clearPrompt();

#endif