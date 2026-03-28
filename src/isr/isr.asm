; src/isr/isr.asm
[bits 32]
section .text

; --- Макросы для исключений (Exceptions 0-31) ---
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push byte 0      ; Фиктивный код ошибки
    push byte %1     ; Номер прерывания
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push byte %1     ; Номер прерывания (код ошибки уже в стеке)
    jmp isr_common_stub
%endmacro

; --- Макрос для аппаратных прерываний (IRQ 32-47) ---
%macro IRQ 2
global irq%1
irq%1:
    cli
    push byte 0      ; Фиктивный код ошибки
    push byte %2     ; Переназначенный номер (32, 33 и т.д.)
    jmp irq_common_stub
%endmacro

; Генерируем 32 обработчика исключений
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8  ; Double Fault
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13 ; General Protection Fault
ISR_ERRCODE   14 ; Page Fault
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

; Генерируем 16 обработчиков IRQ
IRQ 0, 32  ; Таймер
IRQ 1, 33  ; Клавиатура
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

; --- Общие заглушки (Stubs) ---
extern isr_handler
isr_common_stub:
    pusha                    ; Сохраняем EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
    mov ax, ds
    push eax                 ; Сохраняем текущий сегмент данных
    
    mov ax, 0x10             ; Загружаем сегмент данных ядра (0x10 из GDT)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    push esp                 ; Указатель на структуру регистров для Си
    call isr_handler
    
    add esp, 4
    pop eax                  ; Восстанавливаем сегмент данных
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa                     ; Восстанавливаем регистры общего назначения
    add esp, 8               ; Очищаем код ошибки и номер прерывания
    iret                     ; Возврат (прерывания включатся сами через EFLAGS)

extern irq_handler
irq_common_stub:
    pusha
    mov ax, ds
    push eax
    
    mov ax, 0x10             ; Сегмент данных ядра
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    push esp
    call irq_handler
    
    add esp, 4
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    add esp, 8
    iret