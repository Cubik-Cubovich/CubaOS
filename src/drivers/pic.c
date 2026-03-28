#include "pic.h"
#include "io.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define ICW1_ICW4    0x01
#define ICW1_INIT    0x10
#define ICW4_8086    0x01

void pic_init(void) {
    // Начало инициализации
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    
    // Установка векторов прерываний: master 0x20, slave 0x28
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    
    // Сообщаем master о наличии slave (IRQ2)
    outb(PIC1_DATA, 0x04);
    // Сообщаем slave его идентификатор
    outb(PIC2_DATA, 0x02);
    
    // Устанавливаем режим 8086
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);
    
    // Маскируем все прерывания (пока не нужны)
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void pic_mask_irq(uint8_t irq, uint8_t mask) {
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    
    value = inb(port);
    if (mask) {
        value |= (1 << irq);
    } else {
        value &= ~(1 << irq);
    }
    outb(port, value);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, 0x20);
    }
    outb(PIC1_COMMAND, 0x20);
}