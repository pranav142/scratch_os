#pragma once

#include "../utils/memtools.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

#define WHITE_ON_BLACK 0x0f

#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

void clear_screen(void);
void print(char *message);
void print_at(char *message, int col, int row);
void scroll_screen_up();
void handle_scroll_screen();
void printf(const char *format, ...);
void print_number(int num, int radix, bool isSigned);
void print_char(char character, int col, int row);
