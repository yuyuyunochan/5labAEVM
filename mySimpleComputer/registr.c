#include "../include/mySimpleComputer.h"
#include <stdio.h>
#include <stdlib.h>

int
sc_regInit (void)
{
  flags = 0;
  return 0;
}

int
sc_regSet (int register_mask, int value)
{
  if (register_mask < 0 || register_mask >= SC_REG_SIZE)
    {
      printf ("ERROR\t Недопустимый регистр!\n");
      return -1;
    }
  if (value != 0 && value != 1)
    {
      printf ("ERROR\t Некорректное значение флага!\n");
      return -1;
    }

  if (value != 0)
    {
      flags |= (1 << register_mask);
    }
  else
    {
      flags &= ~(1 << register_mask);
    }
  return 0;
}

int
sc_regGet (int register_mask, int *value)
{
  if (value == NULL || (register_mask < 0 || register_mask >= SC_REG_SIZE))
    {
      printf ("ERROR\tНеверный указатель или недопустимый регистр!\n");
      return -1;
    }

  *value = (flags & (1 << register_mask)) ? 1 : 0;
  return 0;
}

int
sc_accumulatorInit (void)
{
  accumulator = 0;
  return 0;
}

int
sc_accumulatorSet (int value)
{
  if (value < -0x7FFF || value > 0x7FFF)
    {
      return -1;
    }
  accumulator = value & 0xFFFF;
  return 0;
}

int
sc_accumulatorGet (int *value)
{
  if (value == NULL)
    {
      printf ("ERROR\tНеверный указатель аккумулятора!\n");
      return -1;
    }

  *value = accumulator;
  return 0;
}

int
sc_icounterInit (void)
{
  icounter = 0;
  return 0;
}

int
sc_icounterSet (int value)
{
  if (value < MIN_VALUE || value > MAX_VALUE)
    {
      printf ("ERROR\tНедопустимое значение ячейки счетчика команд!\n");
      return -1;
    }

  icounter = value;
  return 0;
}

int
sc_icounterGet (int *value)
{
  if (value == NULL)
    {
      printf ("ERROR\tНеверный указатель счетчика команд!\n");
      return -1;
    }

  *value = icounter;
  return 0;
}
