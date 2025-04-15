#ifndef MYBIGCHARS_H
#define MYBIGCHARS_H

#include "myTerm.h"
#include <fcntl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

// Подсчет символов в UTF-8 строке
int bc_strlen (char *str);

// Вывод строки с использованием дополнительной кодировочной таблицы
int bc_printA (char *str);

// Вывод псевдографической рамки
int bc_box (int x1, int y1, int x2, int y2, enum colors box_fg,
            enum colors box_bg, char *header, enum colors header_fg,
            enum colors header_bg);

// Установка позиции в большом символе
int bc_setbigcharpos (int *big, int x, int y, int value);

// Получение позиции в большом символе
int bc_getbigcharpos (int *big, int x, int y, int *value);

// Вывод большого символа
int bc_printbigchar (int big[2], int x, int y, enum colors fg, enum colors bg);

// Запись больших символов в файл
int bc_bigcharwrite (int fd, int *big, int count);

// Чтение больших символов из файла
int bc_bigcharread (int fd, int *big, int need_count, int *count);

#endif
