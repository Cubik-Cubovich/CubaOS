#ifndef ISR_H
#define ISR_H

#include <stdint.h>

// Структура, передаваемая в обработчик
typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

// Указатель на функцию-обработчик
typedef void (*isr_t)(registers_t*);

// Регистрация обработчика для конкретного прерывания (0–255)
void register_interrupt_handler(uint8_t n, isr_t handler);

// Функции, вызываемые из ассемблерных заглушек
void isr_handler(registers_t* regs);
void irq_handler(registers_t* regs);

#endif