#include "screen.h"
#include "ports.h"

int get_screen_offset(int col, int row) { 
    return 2 * (row * MAX_COLS + col);
}

int calculate_cursor_offset(int col, int row) { 
    return (row * MAX_COLS + col);
}

int get_cursor_offset() { 
    port_byte_out(REG_SCREEN_CTRL, 0x0E);
    unsigned char hibyte = port_byte_in(REG_SCREEN_DATA);

    port_byte_out(REG_SCREEN_CTRL, 0x0F);
    unsigned char lowbyte = port_byte_in(REG_SCREEN_DATA);

    int offset = (hibyte << 8) | lowbyte;
    return offset * 2;
}

void set_cursor_offset(int offset) { 

    unsigned char hibyte = (offset >> 8) & 0xFF;
    unsigned char lowbyte = offset & 0xFF;

    port_byte_out(REG_SCREEN_CTRL, 0x0E);
    port_byte_out(REG_SCREEN_DATA, hibyte);

    port_byte_out(REG_SCREEN_CTRL, 0x0F);
    port_byte_out(REG_SCREEN_DATA, lowbyte);

}

void print_char(char character, int col, int row) {
    char *video_memory = (char *) VIDEO_ADDRESS;
    int offset;

    if (col >= 0 && row >= 0) { 
        offset = get_screen_offset(col, row);
    } else { 
        offset = get_cursor_offset();
    }

    if (character=='\n') { 
        row = offset/(2*MAX_COLS);
        offset = get_screen_offset(0, row+1);
    } else if (character=='\t'){ 
        offset += 16;
    } else if (character=='\b'){ 
        if (offset > 0) {
            offset -= 2; 
            video_memory[offset] = ' '; 
            video_memory[offset + 1] = WHITE_ON_BLACK; 
        }
    } else {    
        video_memory[offset] = character;
        video_memory[offset + 1] = WHITE_ON_BLACK;
        offset += 2;
    }

    set_cursor_offset(offset/2);
    // scrolls if cursor is off the screen
    handle_scroll_screen();
}

void print_at(char* message, int col, int row) { 
    int i = 0;

    if (col >= 0 && row >= 0) {
        set_cursor_offset(get_cursor_offset(col,row));
    }

    print_char(message[i++], col, row);
    while (message[i] != 0) { 
        print_char(message[i++], -1, -1);
    }
}

void print(char* message) { 
    print_at(message, -1, -1);
}

void clear_screen() {
    for (int row = 0; row < MAX_ROWS; row++) { 
        for (int col = 0; col < MAX_COLS; col++) { 
            print_char(' ', row, col);
        }
    }

    set_cursor_offset(calculate_cursor_offset(0, 0));
}

void scroll_screen_up() { 
    // move all content by 1 row
    char *video_memory = (char *) VIDEO_ADDRESS;

    for (int row = 0; row < MAX_ROWS - 1; row++) { 
        for (int col = 0; col < MAX_COLS; col ++) { 
            int current_offset = get_screen_offset(col, row);
            int next_offset = get_screen_offset(col, row + 1);
    
            video_memory[current_offset] = video_memory[next_offset];
            video_memory[current_offset + 1] = video_memory[next_offset + 1];
        }
    }

    // erase last line
    int last_line_start = get_screen_offset(0, MAX_ROWS-1);
    for (int col = 0; col < MAX_COLS - 1; col++) { 
        video_memory[last_line_start + col * 2] = ' '; 
        video_memory[last_line_start + col * 2 + 1] = WHITE_ON_BLACK; 
    }
    // moves the cursor up by one
    set_cursor_offset(get_cursor_offset()/2 - MAX_COLS); 
}

void handle_scroll_screen() { 
    int last_line_offset = get_screen_offset(0, MAX_ROWS);
    if (get_cursor_offset() >= last_line_offset) { 
        scroll_screen_up();
    } 
}

void printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++; 
            switch (*format) {
                case 'c': {
                    char c = (char)va_arg(args, int); // Promoted to int
                    print_char(c, -1, -1);
                    break;
                }
                case 's': {
                    char* s = va_arg(args, char*);
                    print(s);
                    break;
                }
                case 'd':
                case 'i': {
                    int i = va_arg(args, int);
                    print_number(i, 10, true); // Base 10, signed
                    break;
                }
                case 'u': {
                    unsigned int u = va_arg(args, unsigned int);
                    print_number(u, 10, false); // Base 10, unsigned
                    break;
                }
                case 'x':
                case 'X': {
                    unsigned int x = va_arg(args, unsigned int);
                    print_number(x, 16, false); // Base 16, unsigned
                    break;
                }
                case 'p': {
                    void* p = va_arg(args, void*);
                    print("0x");
                    print_number((unsigned long)p, 16, false); // Base 16, unsigned
                    break;
                }
                case 'o': {
                    unsigned int o = va_arg(args, unsigned int);
                    print_number(o, 8, false); // Base 8, unsigned
                    break;
                }
                case '%': {
                    print_char('%', -1, -1);
                    break;
                }
            }
        } else {
            print_char(*format, -1, -1);
        }
        format++;
    }

    va_end(args);
}

const char possibleChars[] = "0123456789abcdef"; 

void print_number(int num, int radix, bool isSigned) {
    char buffer[33]; 
    char* ptr = &buffer[sizeof(buffer) - 1]; 
    *ptr = '\0'; 

    if (isSigned && num < 0) {
        print_char('-', -1, -1); 
        num = -num; 
    }

    if (num == 0) {
        print_char('0', -1, -1);
        return;
    }

    while (num != 0) {
        int remainder = num % radix;
        *--ptr = (remainder < 10) ? (char)(remainder + '0') : (char)(remainder - 10 + 'a'); 
        num /= radix;
    }

    // Print the number
    while (*ptr) {
        print_char(*ptr++, -1 , -1);
    }
}






