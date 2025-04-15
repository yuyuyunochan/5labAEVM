#include "../include/mySimpleComputer.h"
#include <stdio.h>
#include <stdlib.h>

#define SIGN_MASK (1 << 14)
#define COMMAND_MASK (0x7F << 7)
#define OPERAND_MASK (0x7F)

int
sc_commandEncode (int sign, int command, int operand, int *value)
{
  if (value == NULL)
    {
      printf ("ERROR\tНеверный указатель на результат\n");
      return -1;
    }

  if (sign != 0 && sign != 1)
    {
      printf ("ERROR\tНедопустимое значение знака\n");
      return -1;
    }
  if (command < 0 || command > 127)
    {
      printf ("ERROR\tНедопустимое значение команды\n");
      return -1;
    }
  if (operand < 0 || operand > 127)
    {
      printf ("ERROR\tНедопустимое значение операнда\n");
      return -1;
    }

  *value = 0;
  for (int i = 0; i < 7; i++)
    {
      int bit = (operand >> i) & 1;
      *value |= (bit << i);
    }
  for (int i = 0; i < 7; i++)
    {
      int bit = (command >> i) & 1;
      *value |= (bit << (i + 7));
    }

  return 0;
}

int
sc_commandDecode (int value, int *sign, int *command, int *operand)
{
  if (sign == NULL || command == NULL || operand == NULL)
    {
      printf ("ERROR\tНеверные указатели на выходные параметры\n");
      return -1;
    }

  // Проверка, что значение соответствует формату команды
  //  if ((value & ~((SIGN_MASK | COMMAND_MASK | OPERAND_MASK))) != 0) {
  //      printf("ERROR\tНедопустимое значение команды\n");
  //      return -1; // Недопустимое значение команды
  //  }

  int tmpCom = 0, tmpOp = 0;
  *sign = value >> 14;

  for (int i = 0; i < 7; i++)
    {
      int bit = (value >> i) & 1;
      tmpOp |= (bit << i);
    }

  for (int i = 0; i < 7; i++)
    {
      int bit = (value >> (i + 7)) & 1;
      tmpCom |= (bit << i);
    }

  *command = tmpCom;
  *operand = tmpOp;

  // *sign = (value & SIGN_MASK) ? 1 : 0;

  // *command = (value & COMMAND_MASK) >> 7;

  // *operand = value & OPERAND_MASK;

  return 0;
}

int
sc_commandValidate (int command)
{
  if (command < 0 || command > 127)
    {
      printf ("ERROR\tНедопустимое значение команды\n");
      return -1;
    }
  return 0;
}
