#include "../include/mySimpleComputer.h"
#include "myBigChars.h"
#include "myReadkey.h"
#include "myTerm.h"
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

void printFlags (void);
void printAccumulator (void);
void printCell (int address, enum colors fg, enum colors bg);
void drawEnlargedCell (int address);
void printCounters (void);
void printCommand (void);
void printDecodedCommand (int value);
void checkAllCommandsInMemory (void);

int nowRedact = 0;
int bigchars[18][2];
int font_loaded = 0;
int tact_counter = 0;
int isFunctionActive = 0;

int
loadFont ()
{
  int fd = open ("font.bin", O_RDONLY);
  if (fd == -1)
    {
      perror ("Error opening font.bin");
      return -1;
    }

  int count;
  for (int i = 0; i < 18; i++)
    {
      if (bc_bigcharread (fd, bigchars[i], 1, &count) != 0 || count != 1)
        {
          close (fd);
          perror ("Error reading from font.bin");
          return -1;
        }
    }

  close (fd);
  font_loaded = 1;
  return 0;
}

int
executeCommand (int command, int operand)
{
  int value;
  int result;

  if (!sc_commandValidate (command))
    {
      sc_regSet (INCORRECT_COMMAND, 1);
      printFlags ();
      return -1;
    }

  switch (command)
    {
    case CMD_NOP:
      break;

    case CMD_CPUINFO:
      mt_gotoXY (23, 1);
      printf ("Simple Computer by [Your Name], Group [Your Group]");
      break;

    case CMD_READ:
      mt_gotoXY (35, 1);
      printf ("Input value for cell %02X: ", operand);
      if (rk_readvalue (&value, 5000) == 0)
        {
          sc_memorySet (operand, value);
        }
      break;

    case CMD_WRITE:
      sc_memoryGet (operand, &value);
      mt_gotoXY (35, 1);
      printf ("Cell %02X value: %c%.4X", operand,
              (value >> 14) & 1 ? '-' : '+', value & 0x7FFF);
      break;

    case CMD_LOAD:
      sc_memoryGet (operand, &value);
      sc_accumulatorSet (value);
      printAccumulator ();
      break;

    case CMD_STORE:
      sc_accumulatorGet (&value);
      sc_memorySet (operand, value);
      printCell (operand, WHITE, WHITE);
      break;

    case CMD_ADD:
      sc_accumulatorGet (&value);
      sc_memoryGet (operand, &result);
      result = (value & 0x7FFF) + (result & 0x7FFF);
      if (result > 0x7FFF)
        {
          sc_regSet (OVERFLOW, 1);
          result &= 0x7FFF;
        }
      sc_accumulatorSet ((value & 0x8000) | result);
      printAccumulator ();
      printFlags ();
      break;
    case CMD_HALT:
      return 1;

    default:
      sc_regSet (INCORRECT_COMMAND, 1);
      printFlags ();
      return -1;
    }

  return 0;
}

void
stepExecution ()
{
  if (isFunctionActive)
    return;
  isFunctionActive = 1;

  checkAllCommandsInMemory ();

  int value = memory[icounter];
  int true_value = value & 0x7FFF;
  int command = (true_value >> 7) & 0x7F;
  int operand = true_value & 0x7F;

  if (!sc_commandValidate (command))
    {
      sc_regSet (INCORRECT_COMMAND, 1);
      printFlags ();
      isFunctionActive = 0;
      return;
    }

  int result = executeCommand (command, operand);

  if (result == 0)
    {
      if (!(command == CMD_JUMP || command == CMD_JNEG || command == CMD_JZ
            || command == CMD_JNS || command == CMD_JC || command == CMD_JNC
            || command == CMD_JP || command == CMD_NP))
        {
          icounter = (icounter + 1) % 128;
        }
    }
  else if (result == 1)
    {
      mt_gotoXY (35, 1);
      printf ("Execution halted at address %02X", icounter);
    }
  else
    {
      mt_gotoXY (35, 1);
      printf ("Error executing command at address %02X", icounter);
    }

  tact_counter++;
  printCounters ();
  printCommand ();
  printFlags ();
  printCell (icounter, WHITE, BG_WHITE);
  drawEnlargedCell (icounter);
  printDecodedCommand (memory[icounter]);

  isFunctionActive = 0;
}

int curent_address = 0;
int in_out_address[MAX_LINES][2] = { 0 };

void
drawBox (int x1, int y1, int width, int height)
{
  mt_gotoXY (y1, x1);
  printf ("┌");
  for (int i = 1; i < width - 1; i++)
    printf ("─");
  printf ("┐");

  for (int i = 1; i < height - 1; i++)
    {
      mt_gotoXY (y1 + i, x1);
      printf ("│");
      mt_gotoXY (y1 + i, x1 + width - 1);
      printf ("│");
    }

  mt_gotoXY (y1 + height - 1, x1);
  printf ("└");
  for (int i = 1; i < width - 1; i++)
    printf ("─");
  printf ("┘");
}

void
printCell (int address, enum colors fg, enum colors bg)
{
  if (address < 0 || address > 127)
    return;

  int value = memory[address];
  int row = 2 + (address / 10);
  int col = 2 + (address % 10) * 6;

  mt_setfgcolor (fg);
  mt_setbgcolor (bg);

  mt_gotoXY (row, col);

  int sign = (value >> 14) & 1;
  int true_value = value & 0x7FFF;
  int command = (true_value >> 7) & 0x7F;
  int operand = true_value & 0x7F;

  printf ("%c%.2X%.2X", sign ? '-' : '+', command, operand);

  mt_setdefaultcolor ();
}

void
printBigCell (int address)
{
  if (address < 0 || address > 127)
    return;

  for (int i = 0; i < 8; i++)
    {
      mt_gotoXY (9 + i, 64);
      printf ("                                        ");
    }

  int value = memory[address];
  int sign = (value >> 14) & 1;
  int true_value = value & 0x7FFF;
  int command = (true_value >> 7) & 0x7F;
  int operand = true_value & 0x7F;

  bc_printbigchar (bigchars[sign ? 17 : 16], 10, 66, WHITE, BLACK);

  char cmd_chars[3];
  snprintf (cmd_chars, sizeof (cmd_chars), "%.2X", command);
  for (int i = 0; i < 2; i++)
    {
      char c = cmd_chars[i];
      if (c >= '0' && c <= '9')
        {
          bc_printbigchar (bigchars[c - '0'], 10, 74 + i * 8, WHITE, BLACK);
        }
      else
        {
          bc_printbigchar (bigchars[10 + (c - 'A')], 10, 74 + i * 8, WHITE,
                           BLACK);
        }
    }

  char op_chars[3];
  snprintf (op_chars, sizeof (op_chars), "%.2X", operand);
  for (int i = 0; i < 2; i++)
    {
      char c = op_chars[i];
      if (c >= '0' && c <= '9')
        {
          bc_printbigchar (bigchars[c - '0'], 10, 90 + i * 8, WHITE, BLACK);
        }
      else
        {
          bc_printbigchar (bigchars[10 + (c - 'A')], 10, 90 + i * 8, WHITE,
                           BLACK);
        }
    }
}

void
drawEnlargedCell (int address)
{
  drawBox (63, 8, 52, 12);
  mt_setbgcolor (BG_WHITE);
  mt_gotoXY (8, 70);
  mt_setfgcolor (RED);
  printf (" Редактируемая ячейка (увеличено) ");
  mt_setdefaultcolor ();
  printBigCell (address);
}

void
printFlags (void)
{
  sc_regSet (IGNORING_TACT_PULSES, 1);
  int value;
  sc_regGet (OVERFLOW, &value);
  char flag_over = (value) ? 'P' : '_';
  sc_regGet (DIVISION_ERR_BY_ZERO, &value);
  char flag_zero = (value) ? '0' : '_';
  sc_regGet (OUT_OF_MEMORY, &value);
  char flag_out = (value) ? 'M' : '_';
  sc_regGet (IGNORING_TACT_PULSES, &value);
  char flag_ign = (value) ? 'T' : '_';
  sc_regGet (INCORRECT_COMMAND, &value);
  char flag_inc = (value) ? 'E' : '_';
  mt_gotoXY (2, 89);
  printf ("    %c  %c  %c  %c  %c", flag_over, flag_zero, flag_out, flag_ign,
          flag_inc);
}

void
checkAllCommands ()
{
  sc_regSet (INCORRECT_COMMAND, 0);

  const int valid_commands[]
      = { CMD_NOP,   CMD_CPUINFO, CMD_READ,   CMD_WRITE, CMD_LOAD, CMD_STORE,
          CMD_ADD,   CMD_SUB,     CMD_DIVIDE, CMD_MUL,   CMD_JUMP, CMD_JNEG,
          CMD_JZ,    CMD_HALT,    CMD_NOT,    CMD_AND,   CMD_OR,   CMD_XOR,
          CMD_JNS,   CMD_JC,      CMD_JNC,    CMD_JP,    CMD_NP,   CMD_CHL,
          CMD_SHR,   CMD_RCL,     CMD_RCR,    CMD_NEG,   CMD_ADDC, CMD_SUBC,
          CMD_LOGLC, CMD_LOGRC,   CMD_RCCL,   CMD_RCCR,  CMD_MOVA, CMD_MOVR,
          CMD_MOVCA, CMD_MOVCR,   CMD_ADDC2,  CMD_SUBC2 };
  const int num_commands
      = sizeof (valid_commands) / sizeof (valid_commands[0]);

  for (int i = 0; i < 128; i++)
    {
      int value = memory[i];
      int true_value = value & 0x7FFF;
      int command = (true_value >> 7) & 0x7F;

      int is_valid = 0;
      for (int j = 0; j < num_commands; j++)
        {
          if (command == valid_commands[j])
            {
              is_valid = 1;
              break;
            }
        }

      if (!is_valid && command != 0)
        {
          sc_regSet (INCORRECT_COMMAND, 1);
          break;
        }
    }

  printFlags ();
}

void
printDecodedCommand (int value)
{
  mt_gotoXY (19, 2);
  int is_negative = (value & 0x8000) ? 1 : 0;
  unsigned int abs_value = (unsigned int)value & 0x7FFF;
  int decimal_value = is_negative ? -((~abs_value + 1) & 0x7FFF) : abs_value;
  printf ("dec: %-6d | oct: %.5o | hex: %.4X\tbin: ", decimal_value, abs_value,
          abs_value);
  for (int i = 14; i >= 0; i--)
    printf ("%d", (abs_value >> i) & 1);
  printf ("      ");
}

void
printAccumulator (void)
{
  mt_gotoXY (2, 64);
  int true_value = accumulator & 0x7FFF;
  int command = (true_value >> 7) & 0x7F;
  int operand = true_value & 0x7F;
  printf ("sc: %c%.2X%.2X hex: %.4X", '+', command, operand,
          accumulator & 0x7FFF);
}

void
printCounters (void)
{
  mt_gotoXY (6, 65);
  int sign, command, operand;
  sc_commandDecode (icounter, &sign, &command, &operand);
  printf ("T:%.2d\tIC: +%.4X", tact_counter % 100, icounter);
}

void
printCommand (void)
{
  int value = memory[icounter];
  int sign = (value >> 14) & 1;
  int true_value = value & 0x7FFF;
  int command = (true_value >> 7) & 0x7F;
  int operand = true_value & 0x7F;

  mt_gotoXY (6, 94);
  const int valid_commands[]
      = { CMD_NOP,   CMD_CPUINFO, CMD_READ,   CMD_WRITE, CMD_LOAD, CMD_STORE,
          CMD_ADD,   CMD_SUB,     CMD_DIVIDE, CMD_MUL,   CMD_JUMP, CMD_JNEG,
          CMD_JZ,    CMD_HALT,    CMD_NOT,    CMD_AND,   CMD_OR,   CMD_XOR,
          CMD_JNS,   CMD_JC,      CMD_JNC,    CMD_JP,    CMD_NP,   CMD_CHL,
          CMD_SHR,   CMD_RCL,     CMD_RCR,    CMD_NEG,   CMD_ADDC, CMD_SUBC,
          CMD_LOGLC, CMD_LOGRC,   CMD_RCCL,   CMD_RCCR,  CMD_MOVA, CMD_MOVR,
          CMD_MOVCA, CMD_MOVCR,   CMD_ADDC2,  CMD_SUBC2 };
  const int num_commands
      = sizeof (valid_commands) / sizeof (valid_commands[0]);

  int is_valid = 0;
  for (int i = 0; i < num_commands; i++)
    {
      if (command == valid_commands[i])
        {
          is_valid = 1;
          break;
        }
    }

  if (!is_valid)
    {
      printf ("!");
      sc_regSet (INCORRECT_COMMAND, 1);
    }
  else
    {
      printf (" ");
      sc_regSet (INCORRECT_COMMAND, 0);
    }

  printf ("%c%.2X:%.2X", sign ? '-' : '+', command, operand);
}

void
printTerms (void)
{
  for (int i = 0; i < 8; i++)
    {
      mt_gotoXY (21 + i, 68);
      int value = memory[i];
      int sign = (value >> 14) & 1;
      int true_value = value & 0x7FFF;
      int command = (true_value >> 7) & 0x7F;
      int operand = true_value & 0x7F;

      printf ("%.4d> %c%.2X%.2X", i, sign ? '-' : '+', command, operand);
    }
}

void
moveCursor (enum keys key)
{
  printCell (curent_address, WHITE, WHITE);

  int new_address = curent_address;
  switch (key)
    {
    case KEY_UP:
      new_address = (curent_address - 10 + 128) % 128;
      break;
    case KEY_DOWN:
      new_address = (curent_address + 10) % 128;
      break;
    case KEY_LEFT:
      new_address = (curent_address - 1 + 128) % 128;
      break;
    case KEY_RIGHT:
      new_address = (curent_address + 1) % 128;
      break;
    default:
      return;
    }

  curent_address = new_address;
  tact_counter++;
  sc_icounterSet (curent_address);

  printCell (curent_address, WHITE, BG_WHITE);
  printBigCell (curent_address);
  printDecodedCommand (memory[curent_address]);
  printCommand ();
  printCounters ();

  fflush (stdout);
}

int inPlaceEdit(int index) {
  if (index < 0 || index > 127)
      return -1;

  int row = 2 + (index / 10);
  int col = 2 + (index % 10) * 6;
  char input[6] = { ' ' }; // Буфер для ввода (5 символов: знак и 4 цифры)
  int pos = 0;             // Текущая позиция курсора
  int original_value;      // Сохраняем текущее значение ячейки
  sc_memoryGet(index, &original_value);

  mt_gotoXY(row, col);
  fflush(stdout);
  rk_mytermregime(1, 0, 1, 1, 1); // Включение режима ввода

  while (1) {
      char c;
      if (read(STDIN_FILENO, &c, 1) <= 0)
          continue;

      if (c == '\n' || c == '\r') { // Завершение ввода при нажатии Enter
          break;
      } else if ((c == 127 || c == '\b') && pos > 0) { // Удаление символа
          pos--;
          input[pos] = ' ';
          mt_gotoXY(row, col + pos);
          putchar(' ');
          mt_gotoXY(row, col + pos);
          fflush(stdout);
      } else if (pos == 0 && (c == '+' || c == '-')) { // Ввод знака
          input[pos] = c;
          mt_gotoXY(row, col + pos);
          putchar(c);
          pos++;
          mt_gotoXY(row, col + pos);
          fflush(stdout);
      } else if (pos > 0 && pos < 5 && isxdigit(c)) { // Ввод шестнадцатеричных цифр
          input[pos] = toupper(c);
          mt_gotoXY(row, col + pos);
          putchar(toupper(c));
          pos++;
          mt_gotoXY(row, col + pos);
          fflush(stdout);
      }
  }

  rk_mytermrestore();

  // Проверяем, что введено корректное значение
  if (input[0] == '+' || input[0] == '-') {
      char hex_value[5];
      strncpy(hex_value, input + 1, 4);
      hex_value[4] = '\0';
      long value = strtol(hex_value, NULL, 16);

      // Проверка диапазона
      if (value >= 0 && value <= 0x7FFF) {
          if (input[0] == '-') {
              value |= 0x8000; // Устанавливаем бит знака
          }
          sc_memorySet(index, value); // Устанавливаем новое значение
      } else {
          // Если значение недопустимо, оставляем старое значение
          sc_memorySet(index, original_value);
          fflush(stdout);
          sleep(1);
      }
  } else {
      // Если ввод некорректный, оставляем старое значение
      sc_memorySet(index, original_value);
  }

  // Обновляем отображение ячейки
  printCell(index, WHITE, BG_WHITE);
  drawEnlargedCell(index);
  printDecodedCommand(memory[index]);
  checkAllCommands();
  mt_setdefaultcolor();

  return 0;
}
void
validateMemoryCommands ()
{
  int has_invalid = 0;

  for (int i = 0; i < MEMORY_SIZE; i++)
    {
      int value = memory[i];
      int command = (value >> 7) & 0x7F;

      if (!sc_commandValidate (command) && command != 0)
        {
          has_invalid = 1;
          break;
        }
    }

  sc_regSet (INCORRECT_COMMAND, has_invalid);
}

// void
// runExecution ()
// {
//   if (isFunctionActive)
//     return;
//   isFunctionActive = 1;

//   while (1)
//     {
//       stepExecution ();
//       sleep (100000);

//       enum keys key;
//       if (rk_readkey (&key))
//         {
//           if (key == KEY_ESC)
//             break;
//         }

//       int value = memory[icounter];
//       int true_value = value & 0x7FFF;
//       int command = (true_value >> 7) & 0x7F;

//       if (command == CMD_HALT)
//         break;
//     }

//   isFunctionActive = 0;
// }

void editAccumulatorInPlace() {
  mt_setfgcolor(WHITE);
  isFunctionActive = 1;

  char input[6] = "     "; // Буфер для ввода (5 символов: знак и 4 цифры)
  int pos = 0;             // Текущая позиция курсора
  int original_value;      // Сохраняем текущее значение аккумулятора
  sc_accumulatorGet(&original_value);

  mt_gotoXY(2, 68);
  fflush(stdout);
  rk_mytermregime(1, 0, 1, 1, 1); // Включение режима ввода

  while (1) {
      char c;
      if (read(STDIN_FILENO, &c, 1) <= 0)
          continue;

      if (c == '\n' || c == '\r') { // Завершение ввода при нажатии Enter
          break;
      } else if ((c == 127 || c == '\b') && pos > 0) { // Удаление символа
          pos--;
          input[pos] = ' ';
          mt_gotoXY(2, 68 + pos);
          putchar(' ');
          mt_gotoXY(2, 68 + pos);
          fflush(stdout);
      } else if (pos == 0 && (c == '+' || c == '-')) { // Ввод знака
          input[pos] = c;
          mt_gotoXY(2, 68 + pos);
          putchar(c);
          pos++;
          mt_gotoXY(2, 68 + pos);
          fflush(stdout);
      } else if (pos > 0 && pos < 5 && isxdigit(c)) { // Ввод шестнадцатеричных цифр
          input[pos] = toupper(c);
          mt_gotoXY(2, 68 + pos);
          putchar(toupper(c));
          pos++;
          mt_gotoXY(2, 68 + pos);
          fflush(stdout);
      }
  }

  rk_mytermrestore();

  // Проверяем, что введено корректное значение
  if (input[0] == '+' || input[0] == '-') {
      char hex_value[5];
      strncpy(hex_value, input + 1, 4);
      hex_value[4] = '\0';
      long value = strtol(hex_value, NULL, 16);

      // Проверка диапазона
      if (value >= 0 && value <= 0x7FFF) {
          if (input[0] == '-') {
              value |= 0x8000; // Устанавливаем бит знака
          }
          sc_accumulatorSet(value); // Устанавливаем новое значение
      } else {
          // Если значение недопустимо, оставляем старое значение
          sc_accumulatorSet(original_value);
          fflush(stdout);
          sleep(1);
      }
  } else {
      // Если ввод некорректный, оставляем старое значение
      sc_accumulatorSet(original_value);
  }

  // Обновляем отображение аккумулятора
  printAccumulator();
  fflush(stdout);
  isFunctionActive = 0;
}

void
editInstructionCounterInPlace ()
{
  mt_setbgcolor (WHITE);
  if (isFunctionActive)
    {
      mt_gotoXY (35, 1);
      printf ("Cannot edit: Function active");
      fflush (stdout);
      sleep (1);
      mt_gotoXY (35, 1);
      printf ("                    ");
      fflush (stdout);
      return;
    }

  isFunctionActive = 1;

  int original_value;
  sc_icounterGet (&original_value);

  char input[6] = "+0000";
  int pos = 0;

  mt_gotoXY (6, 77);
  fflush (stdout);

  rk_mytermregime (1, 0, 1, 1, 0);

  while (1)
    {
      char c;
      if (read (STDIN_FILENO, &c, 1) <= 0)
        continue;

      if (c == '\n')
        {
          break;
        }
      else if ((c == 127 || c == '\b') && pos > 0)
        {
          pos--;
          input[pos] = (pos == 0) ? '+' : '0';
          mt_gotoXY (6, 77 + pos);
          putchar (input[pos]);
          mt_gotoXY (6, 77 + pos);
          fflush (stdout);
        }
      else if (pos == 0 && c == '+')
        {
          input[pos] = c;
          mt_gotoXY (6, 77);
          putchar (c);
          pos++;
          mt_gotoXY (6, 77 + pos);
          fflush (stdout);
        }
      else if (pos > 0 && pos < 5 && isxdigit (c))
        {
          input[pos] = toupper (c);
          mt_gotoXY (6, 77 + pos);
          putchar (toupper (c));
          pos++;
          mt_gotoXY (6, 77 + pos);
          fflush (stdout);
        }
    }

  rk_mytermrestore ();

  if (input[0] == '+' && pos == 5)
    {
      int value;
      char hex_value[5];
      strncpy (hex_value, input + 1, 4);
      hex_value[4] = '\0';
      value = (int)strtol (hex_value, NULL, 16);

      if (value >= 0 && value <= 127)
        {
          sc_icounterSet (value);
          printCounters ();
          printCommand ();
        }
      else
        {
          sc_icounterSet (original_value);
          printCounters ();
          printCommand ();
          fflush (stdout);
          sleep (1);
        }
    }
  else
    {
      sc_icounterSet (original_value);
      printCounters ();
      printCommand ();
      fflush (stdout);
      sleep (1);
    }
  fflush (stdout);

  isFunctionActive = 0;
}

void
loadMemory ()
{
  isFunctionActive = 1;

  char filename[256];
  mt_gotoXY (35, 1);
  mt_setfgcolor (WHITE);
  printf ("Enter filename to load: ");
  rk_mytermregime (0, 0, 0, 1, 1);
  fgets (filename, sizeof (filename), stdin);
  filename[strcspn (filename, "\n")] = '\0';
  rk_mytermrestore ();
  mt_gotoXY (35, 1);
  printf ("                                                                   "
          "      ");
  FILE *file = fopen (filename, "r");
  if (!file)
    {
      mt_gotoXY (35, 1);
      printf ("Failed to open file for loading!");
      fflush (stdout);
      sleep (1);
      isFunctionActive = 0;
      return;
    }

  for (int i = 0; i < MEMORY_SIZE; i++)
    {
      if (fscanf (file, "%d", &memory[i]) != 1)
        {
          fclose (file);
          mt_gotoXY (35, 1);
          printf ("Error reading memory from file!");
          fflush (stdout);
          sleep (1);
          isFunctionActive = 0;
          return;
        }
    }
  char buffer[256];
  int icounter = 0;
  if (fgets (buffer, sizeof (buffer), file))
    {
      if (sscanf (buffer, "IC:%d", &icounter) == 1)
        {
          sc_icounterSet (icounter);
        }
    }

  int accumulator = 0;
  if (fgets (buffer, sizeof (buffer), file))
    {
      if (sscanf (buffer, "ACC:%d", &accumulator) == 1)
        {
          sc_accumulatorSet (accumulator);
        }
    }

  fclose (file);

  for (int i = 0; i < MEMORY_SIZE; i++)
    {
      printCell (i, WHITE, i == curent_address ? BG_WHITE : WHITE);
    }
  printCounters ();
  printAccumulator ();
  drawEnlargedCell (curent_address);

  mt_gotoXY (35, 1);
  printf ("Loaded from ");
  mt_setfgcolor (RED);
  printf ("%s", filename);
  fflush (stdout);
  sleep (1);

  checkAllCommands ();
  isFunctionActive = 0;
}
void
saveMemory ()
{
  isFunctionActive = 1;

  char filename[256];
  mt_setfgcolor (WHITE);
  mt_gotoXY (35, 1);
  printf ("Enter filename to save: ");
  rk_mytermregime (0, 0, 0, 1, 1);
  fgets (filename, sizeof (filename), stdin);
  filename[strcspn (filename, "\n")] = '\0';
  rk_mytermrestore ();
  mt_setfgcolor (WHITE);
  FILE *file = fopen (filename, "w");
  if (!file)
    {
      mt_gotoXY (35, 1);
      printf ("Failed to open file for saving");
      fflush (stdout);
      sleep (1);
      isFunctionActive = 0;
      return;
    }
  mt_gotoXY (35, 1);
  printf ("                                       ");
  for (int i = 0; i < MEMORY_SIZE; i++)
    {
      fprintf (file, "%d\n", memory[i]);
    }

  int icounter;
  sc_icounterGet (&icounter);
  fprintf (file, "IC:%d\n", icounter);

  int accumulator;
  sc_accumulatorGet (&accumulator);
  fprintf (file, "ACC:%d\n", accumulator);

  fclose (file);

  mt_gotoXY (35, 1);
  printf ("Saved to ");
  mt_setfgcolor (RED);
  printf ("%s", filename);
  fflush (stdout);
  sleep (1);

  validateMemoryCommands ();
  isFunctionActive = 0;
}

void
checkAllCommandsInMemory ()
{
  int has_invalid = 0;

  for (int i = 0; i < MEMORY_SIZE; i++)
    {
      int value = memory[i];
      int command = (value >> 7) & 0x7F;
      if (command != 0 && !sc_commandValidate (command))
        {
          has_invalid = 1;
          break;
        }
    }

  sc_regSet (INCORRECT_COMMAND, has_invalid);
  printFlags ();
}

int
main ()
{
  sc_icounterSet (curent_address);
  mt_clrscr ();

  tact_counter = 0;

  if (!font_loaded)
    {
      if (loadFont () != 0)
        {
          perror ("Failed to load font");
          return -1;
        }
    }
  drawBox (1, 1, 61, 16);
  drawBox (63, 1, 25, 3);
  drawBox (88, 1, 25, 3);
  drawBox (63, 5, 25, 4);
  drawBox (88, 5, 25, 4);
  drawBox (1, 18, 61, 3);
  drawBox (63, 8, 52, 12);
  drawBox (1, 21, 61, 12);
  drawBox (63, 20, 21, 13);
  drawBox (84, 20, 31, 13);

  mt_gotoXY (1, 21);
  mt_setfgcolor (RED);
  printf (" RAM ");

  mt_gotoXY (1, 67);
  printf (" Accumulator ");

  mt_gotoXY (1, 93);
  printf (" Flags register ");

  mt_gotoXY (5, 65);
  printf (" Instruction Counter ");

  mt_gotoXY (5, 93);
  printf (" Command ");

  mt_setbgcolor (BG_WHITE);
  mt_gotoXY (8, 68);
  printf (" Modified Cell (increased)");

  mt_gotoXY (18, 16);
  printf (" Modified Cell (format) ");

  mt_setfgcolor (RED);
  mt_gotoXY (21, 25);
  printf (" Cache CPU ");

  mt_gotoXY (20, 68);
  printf (" IN--OUT ");

  mt_gotoXY (20, 91);
  printf (" Keyboard ");

  mt_setdefaultcolor ();

  mt_gotoXY (22, 85);
  printf ("l - load");
  mt_gotoXY (23, 85);
  printf ("s - save");
  mt_gotoXY (24, 85);
  printf ("i - reset");
  mt_gotoXY (25, 85);
  printf ("→ - right");
  mt_gotoXY (26, 85);
  printf ("← - left");
  mt_gotoXY (27, 85);
  printf ("ESC - exit ");
  mt_gotoXY (28, 85);
  printf ("F5 - accumulator ");
  mt_gotoXY (29, 85);
  printf ("F6 - instruction counter");

  srand (time (NULL));

  sc_memoryInit ();
  sc_accumulatorInit ();
  sc_icounterInit ();
  sc_regInit ();

  checkAllCommands ();
  checkAllCommandsInMemory ();
  printFlags ();
  for (int i = 0; i < 10; i++)
    {
      int num = rand () % 32768;
      int adjusted_num = num - 16384;
      sc_memorySet (i, adjusted_num);
    }
  sc_memorySet (0, -1);
  sc_memorySet (1, -16384);
  sc_memorySet (2, 16383);

  for (int i = 0; i < 128; i++)
    {
      sc_memoryGet (i, &memory[i]);
      printCell (i, WHITE, i == curent_address ? BG_WHITE : WHITE);
    }
  validateMemoryCommands ();
  checkAllCommands ();
  sc_accumulatorSet (-1);
  printAccumulator ();
  mt_setbgcolor (WHITE);

  printFlags ();

  printDecodedCommand (memory[curent_address]);

  sc_icounterSet (40);
  printCounters ();
  printCommand ();

  printTerms ();
  drawEnlargedCell (curent_address);

  rk_mytermsave ();

  mt_setfgcolor (WHITE);
  mt_setbgcolor (BLACK);

  enum keys key;
  while (1)
    {
      if (rk_readkey (&key) != 0)
        continue;

      switch (key)
        {
        case KEY_ESC:
          rk_mytermrestore ();
          printf ("\033c");
          return 0;
        case KEY_UP:
        case KEY_DOWN:
        case KEY_LEFT:
        case KEY_RIGHT:
          moveCursor (key);
          break;
        case KEY_ENTER:
          inPlaceEdit (curent_address);
          break;
        case KEY_L:
          loadMemory ();
          break;
        case KEY_S:
          saveMemory ();
          break;
        case KEY_I:
          isFunctionActive = 1;
          sc_memoryInit ();
          curent_address = 0;
          for (int i = 0; i < 128; i++)
            {
              sc_memorySet (i, 0);
              sc_memoryGet (i, &memory[i]);
              printCell (i, WHITE, i == curent_address ? BG_WHITE : WHITE);
            }
          sc_accumulatorInit ();
          sc_icounterInit ();
          sc_regInit ();
          // Set default flags: _ _ _ T _
          sc_regSet (IGNORING_TACT_PULSES, 1);

          tact_counter = 0;
          printAccumulator ();
          printCounters ();
          printFlags ();
          printDecodedCommand (memory[curent_address]);
          printCommand ();
          printTerms ();
          drawEnlargedCell (curent_address);
          isFunctionActive = 0;
          break;
        case KEY_F5:
          editAccumulatorInPlace ();
          break;
        case KEY_F6:
          editInstructionCounterInPlace ();
          break;
        default:
          break;
        }
    }

  rk_mytermrestore ();
  mt_gotoXY (30, 1);
  printf ("\033[0m");
  return 0;
}
