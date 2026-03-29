#include "idt.h"

// Сама таблица на 256 записей
struct idt_entry idt[256];
// Указатель для инструкции LIDT
struct idt_ptr idtp;

// Объявляем внешние обработчики из ассемблерного файла (isr.asm)
// В идеале тут должны быть и обработчики исключений (0-31), 
// но пока пропишем только таймер для теста.
extern void isr0x20(void); 

void idt_init(void) {
    // 1. Настраиваем указатель IDT
    idtp.limit = (sizeof(idt) - 1);
    idtp.base = (uint32_t)&idt;

    // 2. Очищаем таблицу (заполняем нулями)
    // Это важно, чтобы невызванные прерывания не указывали на мусор
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    /* * 3. Настраиваем таймер (IRQ0 -> прерывание 0x20)
     * * КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ:
     * Селектор должен быть 0x08 (это Kernel Code Segment в большинстве GDT).
     * Если поставить 0x10 (Data Segment), процессор выдаст ошибку, так как 
     * он не может "исполнять" сегмент данных.
     *
     * Флаги 0x8E: 
     * 1 (Present) | 00 (Ring 0) | 0 (Storage) | 1110 (32-bit Interrupt Gate)
     */
    extern void irq0(void);
    idt_set_gate(0x20, (uint32_t)irq0, 0x08, 0x8E);
    extern void irq1(void);  // объявление обработчика из isr.asm
    idt_set_gate(0x21, (uint32_t)irq1, 0x08, 0x8E);

    // 4. Загружаем IDT в регистр процессора
    asm volatile ("lidt %0" : : "m" (idtp));
}

// Функция для заполнения одной записи в таблице
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;        // Нижние 16 бит адреса
    idt[num].base_high = (base >> 16) & 0xFFFF; // Верхние 16 бит адреса
    
    idt[num].selector = selector;             // Селектор сегмента кода (0x08)
    idt[num].zero = 0;                        // Всегда 0
    idt[num].flags = flags;                   // Тип шлюза и права доступа
}