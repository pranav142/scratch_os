#pragma once

#include <stdarg.h> 
#include <stdint.h> 
#include <stdbool.h> 

# define VIDEO_ADDRESS 0xb8000
# define MAX_ROWS 25
# define MAX_COLS 80

# define WHITE_ON_BLACK 0x0f

# define REG_SCREEN_CTRL 0x3D4
# define REG_SCREEN_DATA 0x3D5

void clear_screen(void);
void print(char *message);
void print_at(char* message, int col, int row);
void scroll_screen_up();
void handle_scroll_screen();
void printf(const char* format, ...);
void print_number(int num, int radix, bool isSigned);

#define PRINTF_STATE_START 0
#define PRINTF_STATE_LENGTH 1
#define PRINTF_STATE_SHORT 2
#define PRINTF_STATE_LONG 3
#define PRINTF_STATE_END 4 

#define PRINTF_LENGTH_START 0 
#define PRINTF_LENGTH_SHORT_SHORT 1
#define PRINTF_LENGTH_SHORT 2
#define PRINTF_LENGTH_LONG_LONG 3
#define PRINTF_LENGTH_LONG 4
