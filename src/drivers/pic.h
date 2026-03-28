#ifndef PIC_H
#define PIC_H

#include <stdint.h>

void pic_init(void);
void pic_mask_irq(uint8_t irq, uint8_t mask);
void pic_send_eoi(uint8_t irq);

#endif