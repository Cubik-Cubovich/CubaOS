#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

// Упрощённая структура для multiboot2 info
struct multiboot_info {
    uint32_t size;
    uint32_t reserved;
    // далее идут теги, но для простоты мы их игнорируем
};

void kernel_main(uint32_t magic, struct multiboot_info* info);

#endif