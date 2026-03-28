#include "idt.h"

struct idt_entry idt[256];
struct idt_ptr idtp;

extern uint32_t isr0x20;   // обработчик из isr.asm

void idt_init(void) {
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)&idt;

    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // IRQ0 (таймер) – прерывание 0x20
    idt_set_gate(0x20, (uint32_t)&isr0x20, 0x10, 0x8E);  // было 0x08

    asm volatile ("lidt %0" : : "m" (idtp));
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags;
}