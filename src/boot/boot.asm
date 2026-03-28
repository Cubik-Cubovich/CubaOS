; boot.asm – исправленная версия с собственной GDT
section .multiboot
align 8
    dd 0xe85250d6
    dd 0
    dd header_end - header_start
    dd -(0xe85250d6 + 0 + (header_end - header_start))

header_start:
    align 8
    dw 0
    dw 0
    dd 8
header_end:

section .text
global _start
extern kernel_main
extern stack_top

_start:
    ; Загружаем свою GDT
    lgdt [gdt_descriptor]

    ; Переключаемся на селектор данных (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, stack_top

    ; Обновляем CS через far jump
    jmp 0x08:protected_mode

protected_mode:
    ; Сохраняем параметры multiboot
    push ebx
    push eax

    call kernel_main

    cli
    hlt
.loop:
    jmp .loop

; ----- GDT -----
section .data
align 8
gdt_start:
    dq 0                    ; нулевой дескриптор
gdt_code:
    dw 0xFFFF               ; limit low
    dw 0                    ; base low
    db 0                    ; base middle
    db 0x9A                 ; access: present, ring0, code, non-conforming, readable
    db 0xCF                 ; granularity: 4K, 32-bit, limit high = 0xF
    db 0                    ; base high
gdt_data:
    dw 0xFFFF
    dw 0
    db 0
    db 0x92                 ; access: present, ring0, data, writeable
    db 0xCF
    db 0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start