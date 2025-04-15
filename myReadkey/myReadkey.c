#include "../include/myReadkey.h"
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

static struct termios saved_termios;

int
rk_mytermsave (void)
{
  if (tcgetattr (STDIN_FILENO, &saved_termios) == -1)
    {
      perror ("Failed to save terminal settings");
      return -1;
    }
  return 0;
}

int
rk_mytermrestore (void)
{
  if (tcsetattr (STDIN_FILENO, TCSANOW, &saved_termios) == -1)
    {
      perror ("Failed to restore terminal settings");
      return -1;
    }
  return 0;
}

int
rk_mytermregime (int regime, int vtime, int vmin, int echo, int sigint)
{
  struct termios term;
  if (tcgetattr (STDIN_FILENO, &term) == -1)
    {
      perror ("Failed to get terminal settings");
      return -1;
    }

  if (regime == 0)
    {
      term.c_lflag |= (ICANON | ECHO);
    }
  else
    {
      term.c_lflag &= ~ICANON;
      if (echo)
        term.c_lflag |= ECHO;
      else
        term.c_lflag &= ~ECHO;
      if (sigint)
        term.c_lflag |= ISIG;
      else
        term.c_lflag &= ~ISIG;

      term.c_cc[VTIME] = vtime;
      term.c_cc[VMIN] = vmin;
    }

  if (tcsetattr (STDIN_FILENO, TCSANOW, &term) == -1)
    {
      perror ("Failed to set terminal regime");
      return -1;
    }
  return 0;
}

int
rk_readkey (enum keys *key)
{
  char buf[10] = { 0 };
  int n;

  rk_mytermregime (1, 0, 1, 0, 0); // Неканонический режим без эха

  n = read (STDIN_FILENO, buf, sizeof (buf) - 1);
  if (n < 1)
    {
      *key = KEY_OTHER;
      return -1;
    }

  // Обработка одиночных символов
  if (n == 1)
    {
      switch (buf[0])
        {
        case '\n':
          *key = KEY_ENTER;
          break;
        case 27:
          *key = KEY_ESC;
          break;
        case 'l':
          *key = KEY_L;
          break;
        case 's':
          *key = KEY_S;
          break;
        case 'i':
          *key = KEY_I;
          break;
        case 'r':
          *key = KEY_R;
          break;
        case 't':
          *key = KEY_T;
          break;
        default:
          *key = KEY_OTHER;
          break;
        }
      return 0;
    }

  // Обработка escape-последовательностей
  if (buf[0] == 27 && buf[1] == '[')
    {
      if (n == 3)
        {
          switch (buf[2])
            {
            case 'A':
              *key = KEY_UP;
              break;
            case 'B':
              *key = KEY_DOWN;
              break;
            case 'C':
              *key = KEY_RIGHT;
              break;
            case 'D':
              *key = KEY_LEFT;
              break;
            default:
              *key = KEY_OTHER;
              break;
            }
        }
      else if (n == 5 && buf[2] == '1')
        {
          if (buf[3] == '5' && buf[4] == '~')
            *key = KEY_F5;
          else if (buf[3] == '7' && buf[4] == '~')
            *key = KEY_F6;
          else
            *key = KEY_OTHER;
        }
      else
        {
          *key = KEY_OTHER;
        }
      return 0;
    }

  *key = KEY_OTHER;
  return 0;
}

int
rk_readvalue (int *value, int timeout)
{
  char buf[10] = { 0 };
  int n;

  rk_mytermregime (1, timeout / 100, 0, 1, 0);

  n = read (STDIN_FILENO, buf, sizeof (buf) - 1);
  if (n <= 0)
    {
      return -1;
    }
  buf[n] = '\0';

  if (sscanf (buf, "%d", value) != 1)
    {
      return -1;
    }
  return 0;
}
