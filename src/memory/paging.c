#include "paging.h"
#include "../drivers/vga.h"

// Каталог страниц и первая таблица страниц (выровнены на 4 КБ)
static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void paging_init(void) {
    vga_writestring("PAGING: initializing...\n");

    // Обнуляем каталог и таблицу
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0;
        first_page_table[i] = 0;
    }

    // Identity mapping для первых 4 МБ (0x00000000 – 0x003FFFFF)
    for (uint32_t i = 0; i < 1024; i++) {
        // Каждая запись: физический адрес страницы + флаги: present (1), read/write (1)
        first_page_table[i] = (i * 4096) | 0x3;
    }

    // Запись в каталоге для адресов 0–4 МБ указывает на первую таблицу страниц
    page_directory[0] = (uint32_t)&first_page_table | 0x3;

    // Загружаем адрес каталога в CR3
    asm volatile("mov %0, %%cr3" : : "r"(page_directory));

    // Включаем paging (бит PG в CR0)
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;          // установить PG
    asm volatile("mov %0, %%cr0" : : "r"(cr0));

    vga_writestring("PAGING: enabled (identity mapping for first 4MB)\n");
}