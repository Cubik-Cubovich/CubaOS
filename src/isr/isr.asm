; isr.asm – обработчики прерываний
section .text

global isr0x20   ; IRQ0 (таймер)

%macro ISR_NOERR 1
global isr%1
isr%1:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x18          ; правильный селектор данных (было 0x10)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    extern timer_handler
    call timer_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iret
%endmacro

ISR_NOERR 0x20