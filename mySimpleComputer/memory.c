#include "../include/mySimpleComputer.h"
#include <stdio.h>
#include <stdlib.h>

int SC_REG_SIZE = 5;

int memory[MEMORY_SIZE];

int accumulator = 0;
int icounter = 0;
int flags = 0;

int
sc_memoryInit (void)
{
  for (int i = 0; i < MEMORY_SIZE; i++)
    {
      memory[i] = 0;
    }
  return 0;
}

int
sc_memorySet (int address, int value)
{
  if (address < 0 || address >= MEMORY_SIZE)
    {
      sc_regSet (OUT_OF_MEMORY, 1);
      return -1;
    }
  memory[address] = value;
  return 0;
}

int
sc_memoryGet (int address, int *value)
{
  if (address < 0 || address >= MEMORY_SIZE || value == NULL)
    {
      printf ("ERROR\tАдрес выходит за границу памяти!\n");
      return -1;
    }
  *value = memory[address];
  return 0;
}

int
sc_memorySave (const char *filename)
{
  FILE *f = fopen (filename, "wb");
  if (!f)
    return -1;
  fwrite (memory, sizeof (int), 128, f);
  fclose (f);
  return 0;
}

int
sc_memoryLoad (const char *filename)
{
  FILE *f = fopen (filename, "rb");
  if (!f)
    return -1;
  fread (memory, sizeof (int), 128, f);
  fclose (f);
  return 0;
}
