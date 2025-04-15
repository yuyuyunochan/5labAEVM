#include "../include/myTerm.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

// Очистка экрана и перемещение курсора в левый верхний угол
int
mt_clrscr (void)
{
  // const char *clear_seq = "\033[2J\033[H";
  // if (write(STDOUT_FILENO, clear_seq, 7) != 7) {
  //     return -1; // Ошибка записи
  // }
  printf ("\033[2J\033[H");
  return 0;
}

// Перемещение курсора в указанную позицию
int
mt_gotoXY (int row, int col)
{
  // char buf[32];
  // int len = snprintf(buf, sizeof(buf), "\033[%d;%dH", row, col);
  // if (write(STDOUT_FILENO, buf, len) != len) {
  //     return -1; // Ошибка записи
  // }
  printf ("\033[%d;%dH", row, col);
  return 0;
}

// Определение размера экрана
int
mt_getscreensize (int *rows, int *cols)
{
  struct winsize ws;
  if (ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
    {
      return -1; // Ошибка получения размеров
    }
  *rows = ws.ws_row;
  *cols = ws.ws_col;
  return 0;
}

// Установка цвета текста
int
mt_setfgcolor (enum colors color)
{
  // char buf[32];
  // int len = snprintf(buf, sizeof(buf), "\033[%dm", color);
  // if (write(STDOUT_FILENO, buf, len) != len) {
  //     return -1; // Ошибка записи
  // }
  printf ("\033[%dm", color);
  return 0;
}

// Установка цвета фона
int
mt_setbgcolor (enum colors color)
{
  // char buf[32];
  // int len = snprintf(buf, sizeof(buf), "\033[%dm", color);
  // if (write(STDOUT_FILENO, buf, len) != len) {
  //     return -1; // Ошибка записи
  // }
  printf ("\033[%dm", color);
  return 0;
}

// Возвращение к стандартным цветам
int
mt_setdefaultcolor (void)
{
  // const char *reset_seq = "\033[0m";
  // if (write(STDOUT_FILENO, reset_seq, 4) != 4) {
  //     return -1; // Ошибка записи
  // }
  printf ("\033[0m");
  return 0;
}

// Показать/скрыть курсор
int
mt_setcursorvisible (int value)
{
  // const char *show_seq = "\033[?25h"; // Показать курсор
  // const char *hide_seq = "\033[?25l"; // Скрыть курсор
  // const char *seq = (value ? show_seq : hide_seq);
  // if (write(STDOUT_FILENO, seq, 6) != 6) {
  //     return -1; // Ошибка записи
  // }
  printf (value ? "\033[?25h" : "\033[?25l");
  return 0;
}

// Очистка текущей строки
int
mt_delline (void)
{
  // const char *clear_line_seq = "\033[2K";
  // if (write(STDOUT_FILENO, clear_line_seq, 4) != 4) {
  //     return -1; // Ошибка записи
  // }
  printf ("\033[2K");
  return 0;
}
