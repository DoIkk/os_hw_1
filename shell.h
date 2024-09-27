#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX 1024
#define DELIMS "\r\t\n "

extern char *extern_cmd_arr[];

// 함수 선언
bool execute(char *cmd);
bool is_in_path(char *argv[100], char *final_path);
int tokenize(char *cmd, char *argv[100]);
bool is_in_arr(char *element, char *arr[], int size);
bool paramCheck(int tokensize);
void printHelp(void);
void printPrompt(void);

#endif