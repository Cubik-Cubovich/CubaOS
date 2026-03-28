#include "isr.h"
#include "idt.h"
#include "pic.h"
#include "vga.h"   // для вывода сообщений об ошибках
#include <stddef.h>

// Массив указателей на обработчики (по умолчанию NULL)
isr_t interrupt_handlers[256];

// Регистрация обработчика
void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

// Общий обработчик для исключений (0–31)
void isr_handler(registers_t* regs) {
    // Если есть зарегистрированный обработчик – вызываем его
    if (interrupt_handlers[regs->int_no] != NULL) {
        interrupt_handlers[regs->int_no](regs);
    } else {
        // Для исключений – выводим номер и останавливаемся
        vga_putstr("Unhandled exception: ");
        vga_puthex(regs->int_no);
        vga_putchar('\n');
        for(;;);
    }
}

// Общий обработчик для аппаратных прерываний (IRQ)
void irq_handler(registers_t* regs) {
    // Если есть обработчик – вызываем
    if (interrupt_handlers[regs->int_no] != NULL) {
        interrupt_handlers[regs->int_no](regs);
    }
    // После обработки посылаем EOI в PIC
    if (regs->int_no >= 40) { // если прерывание от slave (IRQ8–15)
        pic_send_eoi(regs->int_no - 32);
    } else {
        pic_send_eoi(regs->int_no - 32);
    }
}