#include "keyboard.h"
#include "isr.h"
#include "pic.h"
#include "io.h"
#include <stddef.h>

#define KEYBOARD_IRQ     1
#define KEYBOARD_INT_NUM 33     // IRQ1 после ремапа в PIC (0x20 + 1)

#define KEYBUF_SIZE 64

static volatile char keybuf[KEYBUF_SIZE];
static volatile int keybuf_head = 0;
static volatile int keybuf_tail = 0;

static int shift_pressed = 0;
static int caps_lock_on = 0;

// Таблицы сканкодов (Set 1)
static const char keymap_normal[] = {
    0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,   0,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,   0,   'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

static const char keymap_shift[] = {
    0,   0,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,   0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,   0,   'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,   '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

static void keyboard_handler(registers_t *regs) {
    (void)regs;
    uint8_t scancode = inb(0x60);

    // Отпускание клавиши (бит 0x80)
    if (scancode & 0x80) {
        uint8_t code = scancode & 0x7F;
        if (code == 0x2A || code == 0x36) {
            shift_pressed = 0;
        }
        return;
    }

    // Нажатие
    switch (scancode) {
        case 0x2A: case 0x36:               // Left/Right Shift
            shift_pressed = 1;
            break;
        case 0x3A:                          // Caps Lock
            caps_lock_on = !caps_lock_on;
            // Можно добавить мигание светодиода Caps Lock (опционально)
            break;
        case 0x1C:                          // Enter
            {
                int next = (keybuf_head + 1) % KEYBUF_SIZE;
                if (next != keybuf_tail) {
                    keybuf[keybuf_head] = '\n';
                    keybuf_head = next;
                }
            }
            break;
        case 0x0E:                          // Backspace
            {
                int next = (keybuf_head + 1) % KEYBUF_SIZE;
                if (next != keybuf_tail) {
                    keybuf[keybuf_head] = '\b';
                    keybuf_head = next;
                }
            }
            break;
        case 0x0F:                          // Tab
            {
                int next = (keybuf_head + 1) % KEYBUF_SIZE;
                if (next != keybuf_tail) {
                    keybuf[keybuf_head] = '\t';
                    keybuf_head = next;
                }
            }
            break;
        // F1–F12 (игнорируем, можно добавить позже)
        case 0x3B: case 0x3C: case 0x3D: case 0x3E:
        case 0x3F: case 0x40: case 0x41: case 0x42:
        case 0x43: case 0x44: case 0x57: case 0x58:
            break;
        default:
            if (scancode < sizeof(keymap_normal) / sizeof(keymap_normal[0])) {
                char c = shift_pressed ? keymap_shift[scancode] : keymap_normal[scancode];
                if (caps_lock_on && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
                    c ^= 0x20;   // инвертировать регистр
                }
                if (c != 0) {
                    int next = (keybuf_head + 1) % KEYBUF_SIZE;
                    if (next != keybuf_tail) {
                        keybuf[keybuf_head] = c;
                        keybuf_head = next;
                    }
                }
            }
            break;
    }
}

void keyboard_init(void) {
    register_interrupt_handler(KEYBOARD_INT_NUM, keyboard_handler);
    pic_mask_irq(KEYBOARD_IRQ, 0);    // размаскировать IRQ1
}

int keyboard_haschar(void) {
    return keybuf_head != keybuf_tail;
}

char keyboard_getchar(void) {
    while (keybuf_head == keybuf_tail) {
        asm volatile("hlt");          // ждать прерывания
    }
    char c = keybuf[keybuf_tail];
    keybuf_tail = (keybuf_tail + 1) % KEYBUF_SIZE;
    return c;
}