#include "debug.h"
#include "../drivers/vga.h"
#include <stdarg.h>

// По умолчанию показываем INFO, WARN, ERROR
int dbg_level = DBG_LVL_INFO;

static const char* level_str[] = {"ERR", "WRN", "INF", "DBG"};

void dbg_print(int level, const char* file, int line, const char* fmt, ...) {
    if (level > dbg_level) return;

    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_putstr("[");
    vga_putstr(level_str[level]);
    vga_putstr("] ");

    // Вывести имя файла и номер строки (коротко)
    vga_putstr(file);
    vga_putchar(':');
    vga_puthex(line);
    vga_putstr(": ");

    // Форматированный вывод (упрощённый, только строки и числа)
    va_list args;
    va_start(args, fmt);
    for (const char* p = fmt; *p; p++) {
        if (*p == '%') {
            p++;
            if (*p == 's') {
                vga_putstr(va_arg(args, const char*));
            } else if (*p == 'x') {
                vga_puthex(va_arg(args, uint32_t));
            } else if (*p == 'd') {
                // Очень простой вывод int (без itoa, для отладки можно так)
                int num = va_arg(args, int);
                // Временно заглушка: выводим как hex
                vga_putstr("0x");
                vga_puthex((uint32_t)num);
            } else if (*p == 'c') {
                char c = (char)va_arg(args, int);
                vga_putchar(c);
            } else {
                vga_putchar('%');
                vga_putchar(*p);
            }
        } else {
            vga_putchar(*p);
        }
    }
    va_end(args);
    vga_putchar('\n');
}