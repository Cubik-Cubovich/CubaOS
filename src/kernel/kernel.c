#include "kernel.h"
#include "../drivers/vga.h"
#include "../drivers/idt.h"
#include "../drivers/pic.h"
#include "../drivers/timer.h"
#include "../drivers/isr.h"   // <-- добавить

void kernel_main(uint32_t magic, struct multiboot_info* info) {
    (void)magic;
    (void)info;

    vga_init();
    vga_writestring("VGA OK");
    vga_putchar('\n');
    idt_init();      // загружаем IDT
    vga_writestring("IDT OK");
    vga_putchar('\n');
    pic_init();      // ремапим PIC на 0x20-0x2F
    vga_writestring("PIC OK");
    vga_putchar('\n');
    timer_init(100); // инициализируем таймер (внутри размаскирует IRQ0)
    vga_writestring("TIMER OK");
    vga_putchar('\n');

    asm volatile("sti"); // разрешаем прерывания

    vga_putchar('O');
    vga_putchar('K');
    vga_putchar('\n');

    while(1) {
        asm volatile("hlt");
    }
}