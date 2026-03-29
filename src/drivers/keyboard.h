#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);
char keyboard_getchar(void);   // блокирующее получение символа
int keyboard_haschar(void);    // проверка наличия символа в буфере

#endif