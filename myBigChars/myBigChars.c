#include "../include/myBigChars.h"
#include <locale.h>
#include <wchar.h>

int
bc_strlen (char *str)
{
  if (!str)
    return 0;

  setlocale (LC_ALL, "");
  mbstate_t state = { 0 };
  const char *ptr = str;
  size_t len = 0;

  while (*ptr)
    {
      wchar_t wc;
      size_t res = mbrtowc (&wc, ptr, MB_CUR_MAX, &state);

      if (res == (size_t)-1 || res == (size_t)-2)
        {
          return 0; // Некорректный UTF-8 символ
        }

      len++;
      ptr += res;
    }

  return len;
}

int
bc_printA (char *str)
{
  if (!str)
    return -1;

  printf ("\033(0%s\033(B", str);
  return 0;
}

int
bc_box (int x1, int y1, int x2, int y2, enum colors box_fg, enum colors box_bg,
        char *header, enum colors header_fg, enum colors header_bg)
{
  if (x1 < 0 || y1 < 0 || x2 <= 0 || y2 <= 0)
    return -1;

  // Установка цветов
  printf ("\033[3%dm\033[4%dm", box_fg, box_bg);

  // Верхняя граница
  printf ("\033[%d;%dH", x1, y1);
  bc_printA ("l");
  for (int i = 0; i < y2 - 2; i++)
    bc_printA ("q");
  bc_printA ("k");

  // Боковые границы
  for (int i = 1; i < x2 - 1; i++)
    {
      printf ("\033[%d;%dH", x1 + i, y1);
      bc_printA ("x");
      printf ("\033[%d;%dH", x1 + i, y1 + y2 - 1);
      bc_printA ("x");
    }

  // Нижняя граница
  printf ("\033[%d;%dH", x1 + x2 - 1, y1);
  bc_printA ("m");
  for (int i = 0; i < y2 - 2; i++)
    bc_printA ("q");
  bc_printA ("j");

  // Заголовок
  if (header && bc_strlen (header) > 0)
    {
      int header_len = bc_strlen (header);
      int header_pos = y1 + (y2 - header_len) / 2;

      if (header_pos + header_len <= y1 + y2 - 1)
        {
          printf ("\033[%d;%dH\033[3%dm\033[4%dm", x1, header_pos, header_fg,
                  header_bg);
          printf ("%s", header);
        }
    }

  printf ("\033[0m");
  return 0;
}

int
bc_setbigcharpos (int *big, int x, int y, int value)
{
  if (!big || x < 0 || x > 7 || y < 0 || y > 7)
    return -1;

  int index = x / 4;
  int bit = (x % 4) * 8 + y;

  if (value)
    {
      big[index] |= (1 << bit);
    }
  else
    {
      big[index] &= ~(1 << bit);
    }

  return 0;
}

int
bc_getbigcharpos (int *big, int x, int y, int *value)
{
  if (!big || !value || x < 0 || x > 7 || y < 0 || y > 7)
    return -1;

  int index = x / 4;
  int bit = (x % 4) * 8 + (7 - y); // Исправлено для правильного порядка

  *value = (big[index] >> bit) & 1;
  return 0;
}

int
bc_printbigchar (int big[2], int x, int y, enum colors fg, enum colors bg)
{
  if (!big || x < 0 || y < 0)
    return -1;

  printf ("\033[3%dm\033[4%dm", fg, bg);

  for (int i = 0; i < 8; i++)
    {
      printf ("\033[%d;%dH", x + i, y);

      int value;
      for (int j = 0; j < 8; j++)
        {
          bc_getbigcharpos (big, i, j, &value);
          printf (value ? "\u2588" : " ");
        }
    }

  printf ("\033[0m");
  return 0;
}

int
bc_bigcharwrite (int fd, int *big, int count)
{
  if (fd < 0 || !big || count <= 0)
    return -1;

  ssize_t written = write (fd, big, count * 2 * sizeof (int));
  if (written != count * 2 * sizeof (int))
    return -1;

  return 0;
}

int
bc_bigcharread (int fd, int *big, int need_count, int *count)
{
  if (fd < 0 || !big || need_count <= 0 || !count)
    return -1;

  *count = 0;
  ssize_t read_bytes = read (fd, big, need_count * 2 * sizeof (int));

  if (read_bytes <= 0)
    return -1;
  if (read_bytes % (2 * sizeof (int)) != 0)
    return -1;

  *count = read_bytes / (2 * sizeof (int));
  return 0;
}
