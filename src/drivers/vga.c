#include "vga.h"

static uint16_t* vga_buffer = (uint16_t*)VGA_ADDRESS;
static int vga_x = 0;
static int vga_y = 0;
static uint8_t vga_color = 0;

void vga_init(void) {
    vga_clear();
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

void vga_clear(void) {
    uint16_t blank = (vga_color << 8) | ' ';
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = blank;
    }
    vga_x = 0;
    vga_y = 0;
}

void vga_set_color(uint8_t fg, uint8_t bg) {
    vga_color = (bg << 4) | fg;
}

void vga_putchar(char c) {
    if (c == '\n') {
        vga_x = 0;
        vga_y++;
    } else if (c == '\b') {
        if (vga_x > 0) {
            vga_x--;
            uint16_t blank = (vga_color << 8) | ' ';
            vga_buffer[vga_y * VGA_WIDTH + vga_x] = blank;
        }
    } else if (c == '\t') {
        int next_tab = (vga_x + 4) & ~3;
        while (vga_x < next_tab && vga_x < VGA_WIDTH) {
            vga_putchar(' ');
        }
    } else if (c == '\r') {
        vga_x = 0;
    } else {
        uint16_t entry = (vga_color << 8) | c;
        vga_buffer[vga_y * VGA_WIDTH + vga_x] = entry;
        vga_x++;
        if (vga_x >= VGA_WIDTH) {
            vga_x = 0;
            vga_y++;
        }
    }
    
    // Прокрутка (остаётся без изменений)
    if (vga_y >= VGA_HEIGHT) {
        for (int y = 1; y < VGA_HEIGHT; y++) {
            for (int x = 0; x < VGA_WIDTH; x++) {
                vga_buffer[(y-1)*VGA_WIDTH + x] = vga_buffer[y*VGA_WIDTH + x];
            }
        }
        uint16_t blank = (vga_color << 8) | ' ';
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[(VGA_HEIGHT-1)*VGA_WIDTH + x] = blank;
        }
        vga_y = VGA_HEIGHT - 1;
    }
}

void vga_writestring(const char* str) {
    while (*str) {
        vga_putchar(*str++);
    }
}
// Вывод строки
void vga_putstr(const char* str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

// Вывод 32-битного числа в шестнадцатеричном виде
void vga_puthex(uint32_t num) {
    vga_putchar('0');
    vga_putchar('x');
    // Выводим 8 шестнадцатеричных цифр (32 бита)
    for (int i = 28; i >= 0; i -= 4) {
        uint8_t digit = (num >> i) & 0xF;
        vga_putchar(digit < 10 ? '0' + digit : 'A' + digit - 10);
    }
}