#include "kernel.h"
#include "../drivers/vga.h"
#include "../drivers/pic.h"
#include "../drivers/idt.h"
#include "../drivers/timer.h"

void kernel_main(uint32_t magic, struct multiboot_info* info) {
    vga_init();
    vga_clear();
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_writestring("CubaOS v0.1\n");
    vga_writestring("Kernel loaded successfully.\n");

    if (magic != 0x36d76289) {
        vga_writestring("Warning: Multiboot2 magic number incorrect.\n");
    } else {
        vga_writestring("Multiboot2 info verified.\n");
    }

    pic_init();
    vga_writestring("PIC initialized.\n");

    idt_init();
    vga_writestring("IDT initialized.\n");

    timer_init(10);           // частота 100 Гц
    vga_writestring("Timer initialized (100 Hz).\n");

    asm volatile("sti");
    vga_writestring("Interrupts enabled.\n");

    while (1) {
        asm volatile("hlt");
    }
}