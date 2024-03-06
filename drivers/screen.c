#include "screen.h"

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
    }
    else {    
        video_memory[offset] = character;
        video_memory[offset + 1] = WHITE_ON_BLACK;
        offset += 2;
    }

    set_cursor_offset(offset/2);
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
