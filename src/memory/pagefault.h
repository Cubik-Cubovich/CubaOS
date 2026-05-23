#ifndef PAGE_FAULT_H
#define PAGE_FAULT_H

#include "../drivers/isr.h"

void page_fault_handler(registers_t *regs);

#endif