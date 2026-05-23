#include "pagefault.h"
#include "../drivers/vga.h"

// Расшифровка битов error code
static const char* decode_error(uint32_t err) {
    if (err & 0x1) {
        if (err & 0x2) return "Supervisor writing to non-present page";
        else           return "Supervisor reading from non-present page";
    } else {
        if (err & 0x2) return "User writing to non-present page";
        else           return "User reading from non-present page";
    }
}

void page_fault_handler(registers_t *regs) {
    uint32_t fault_addr;
    asm volatile("mov %%cr2, %0" : "=r"(fault_addr));

    // Красный текст на чёрном фоне для паники
    vga_set_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
    vga_writestring("\n\n*** PAGE FAULT ***\n");
    vga_writestring("Address: ");
    vga_puthex(fault_addr);
    vga_writestring("\nError code: ");
    vga_puthex(regs->err_code);
    vga_writestring(" (");
    vga_writestring(decode_error(regs->err_code));
    vga_writestring(")\n");
    vga_writestring("EIP: ");
    vga_puthex(regs->eip);
    vga_writestring("\nCS: ");
    vga_puthex(regs->cs);
    vga_writestring("\nEFLAGS: ");
    vga_puthex(regs->eflags);
    vga_writestring("\n\nSystem halted.\n");

    // Зависаем навсегда
    for(;;) asm volatile("hlt");
}