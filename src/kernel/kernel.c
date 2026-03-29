#include "kernel.h"
#include "../drivers/vga.h"
#include "../drivers/idt.h"
#include "../drivers/pic.h"
#include "../drivers/timer.h"
#include "../drivers/isr.h"
#include "../drivers/keyboard.h"
#include "../shell/shell.h"

void kernel_main(uint32_t magic, struct multiboot_info* info) {
    (void)magic;
    (void)info;

    vga_init();
    vga_writestring("VGA OK\n");
    idt_init();
    vga_writestring("IDT OK\n");
    pic_init();
    vga_writestring("PIC OK\n");
    timer_init(100);
    vga_writestring("TIMER OK\n");
    keyboard_init();
    vga_writestring("KEYBOARD OK\n");

    asm volatile("sti");   // разрешить прерывания

    shell_init();
    shell_run();           // никогда не возвращается
}