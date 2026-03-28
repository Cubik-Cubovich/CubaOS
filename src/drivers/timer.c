#include "timer.h"
#include "io.h"
#include "../drivers/vga.h"
#include "../drivers/pic.h"

volatile uint32_t tick_count = 0;

void timer_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;

    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);

    // Размаскировать IRQ0 в PIC
    outb(0x21, inb(0x21) & 0xFE);
}

void timer_handler(void) {
    tick_count++;
    if (tick_count % 100 == 0) {
        vga_putchar('*');
    }
    pic_send_eoi(0);
}