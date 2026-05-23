#include "isr.h"
#include "idt.h"
#include "pic.h"
#include "../drivers/vga.h"   // для вывода сообщений об ошибках
#include <stddef.h>

// Таблица имён исключений для векторов 0–31
static const char* exception_names[32] = {
    "Division Error",                 // 0
    "Debug",                          // 1
    "NMI",                            // 2
    "Breakpoint",                     // 3
    "Overflow",                       // 4
    "Bound Range Exceeded",           // 5
    "Invalid Opcode",                 // 6
    "Device Not Available",           // 7
    "Double Fault",                   // 8
    "Coprocessor Segment Overrun",    // 9
    "Invalid TSS",                    // 10
    "Segment Not Present",            // 11
    "Stack Fault",                    // 12
    "General Protection Fault",       // 13
    "Page Fault",                     // 14
    "Reserved",                       // 15
    "x87 FPU Error",                  // 16
    "Alignment Check",                // 17
    "Machine Check",                  // 18
    "SIMD Floating-Point Exception",  // 19
    "Virtualization Exception",       // 20
    "Control Protection Exception",   // 21
    "Reserved",                       // 22
    "Reserved",                       // 23
    "Reserved",                       // 24
    "Reserved",                       // 25
    "Reserved",                       // 26
    "Reserved",                       // 27
    "Reserved",                       // 28
    "Reserved",                       // 29
    "Reserved",                       // 30
    "Reserved"                        // 31
};

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
        return;
    }

    // Необработанное исключение – паника с красным текстом
    vga_set_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
    vga_writestring("\n*** UNHANDLED EXCEPTION ***\n");

    if (regs->int_no < 32) {
        vga_writestring(exception_names[regs->int_no]);
    } else {
        vga_writestring("Unknown exception");
    }
    vga_writestring(" (vector ");
    vga_puthex(regs->int_no);
    vga_writestring(")\n");

    // Для исключений, которые передают код ошибки
    if (regs->int_no == 8 || (regs->int_no >= 10 && regs->int_no <= 14) || regs->int_no == 17) {
        vga_writestring("Error code: ");
        vga_puthex(regs->err_code);
        vga_writestring("\n");
    }

    // Специальные подсказки для частых проблем
    if (regs->int_no == 13) {
        vga_writestring("Likely cause: segment violation, ring mismatch, or invalid instruction.\n");
    } else if (regs->int_no == 8) {
        vga_writestring("Double fault: previous exception was not handled.\n");
    }

    // Покажем ключевые регистры для отладки
    vga_writestring("EIP: "); vga_puthex(regs->eip);
    vga_writestring("  CS: "); vga_puthex(regs->cs);
    vga_writestring("  EFLAGS: "); vga_puthex(regs->eflags);
    vga_writestring("\n");
    vga_writestring("EAX: "); vga_puthex(regs->eax);
    vga_writestring("  EBX: "); vga_puthex(regs->ebx);
    vga_writestring("  ECX: "); vga_puthex(regs->ecx);
    vga_writestring("  EDX: "); vga_puthex(regs->edx);
    vga_writestring("\n");
    vga_writestring("ESP: "); vga_puthex(regs->esp);
    vga_writestring("  EBP: "); vga_puthex(regs->ebp);
    vga_writestring("\n");

    vga_writestring("System halted.\n");
    for(;;) asm volatile("hlt");
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