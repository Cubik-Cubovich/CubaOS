#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

// Уровни важности (константы)
#define DBG_LVL_ERROR 0
#define DBG_LVL_WARN  1
#define DBG_LVL_INFO  2
#define DBG_LVL_DEBUG 3

// Глобальный уровень (можно менять из кода)
extern int dbg_level;

// Функция вывода отладочного сообщения
void dbg_print(int level, const char* file, int line, const char* fmt, ...);

// Удобные макросы для вывода
#define DBG_ERR(...)  dbg_print(DBG_LVL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define DBG_WARN(...) dbg_print(DBG_LVL_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define DBG_INFO(...) dbg_print(DBG_LVL_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define DBG_DEBUG(...) dbg_print(DBG_LVL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

#endif