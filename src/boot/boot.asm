; boot.asm – multiboot2-совместимая загрузочная заглушка
; NASM

section .multiboot
align 8
    dd 0xe85250d6                 ; магическое число multiboot2
    dd 0                          ; архитектура 0 (i386)
    dd header_end - header_start  ; длина заголовка
    dd -(0xe85250d6 + 0 + (header_end - header_start)) ; контрольная сумма

header_start:
    ; здесь могут быть дополнительные теги, но пока ничего
    ; завершающий тег
    align 8
    dw 0    ; тип
    dw 0    ; флаги
    dd 8    ; размер
header_end:

section .text
global _start
extern kernel_main
extern stack_top

_start:
    ; установка стека
    mov esp, stack_top

    ; передача параметров в kernel_main
    push ebx    ; указатель на multiboot info
    push eax    ; магическое число

    call kernel_main

    ; если kernel_main вернулся – зависаем
    cli
    hlt
.loop:
    jmp .loop