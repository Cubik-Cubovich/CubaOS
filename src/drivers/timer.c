#include "timer.h"
#include "io.h"
#include "../drivers/vga.h"
#include "isr.h"
#include "pic.h"          // <-- добавить

volatile uint32_t tick_count = 0;

static void timer_callback(registers_t *regs) {
    (void)regs;
    tick_count++;
    if (tick_count % 100 == 0) {
        vga_putchar('*');
    }
}

void timer_init(uint32_t frequency) {
    register_interrupt_handler(32, timer_callback);

    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);

    // Размаскировать IRQ0 в PIC
    pic_mask_irq(0, 0);   // <-- раскомментировать и убедиться, что pic.h подключён
}