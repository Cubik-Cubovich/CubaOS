#include "shell.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include <stddef.h>

#define INPUT_BUFFER_SIZE 256

static char input_buffer[INPUT_BUFFER_SIZE];
static int input_len = 0;

static void echo_input(void) {
    vga_putchar('\n');
    vga_putstr(input_buffer);
    vga_putchar('\n');
    input_len = 0;
    input_buffer[0] = '\0';
}

static void handle_char(char c) {
    if (c == '\n') {
        echo_input();
        vga_putstr("$ ");
    } else if (c == '\b') {
        if (input_len > 0) {
            input_len--;
            input_buffer[input_len] = '\0';
            vga_putchar('\b');
        }
    } else {
        if (input_len < INPUT_BUFFER_SIZE - 1) {
            input_buffer[input_len++] = c;
            input_buffer[input_len] = '\0';
            vga_putchar(c);
        }
    }
}

void shell_init(void) {
    vga_putchar('\n');
    vga_writestring("Welcome to CubaOS shell!\n");
    vga_writestring("Type something and press Enter to echo.\n");
    vga_putstr("$ ");
}

void shell_run(void) {
    while (1) {
        char c = keyboard_getchar();
        handle_char(c);
    }
}