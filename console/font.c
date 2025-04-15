#include "../include/myBigChars.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Массив больших символов для цифр 0-9, A-F, + и !
int BigChars[18][2] = {
  // 0
  { 0x3C66666E, 0x7666663C },
  // 1
  { 0x18381818, 0x1818187E },
  // 2
  { 0x3C66060C, 0x1830607E },
  // 3
  { 0x3C66061C, 0x0666663C },
  // 4
  { 0x0C1C3C6C, 0xCCFE0C0C },
  // 5
  { 0x7E607C06, 0x0666663C },
  // 6
  { 0x3C66607C, 0x6666663C },
  // 7
  { 0x7E660C0C, 0x18181818 },
  // 8
  { 0x3C66663C, 0x6666663C },
  // 9
  { 0x3C66663E, 0x0666663C },
  // A
  { 0x3C66667E, 0x66666666 },
  // B
  { 0x7C66667C, 0x6666667C },
  // C
  { 0x3C666060, 0x6060663C },
  // D
  { 0x7C666666, 0x6666667C },
  // E
  { 0x7E60607C, 0x6060607E },
  // F
  { 0x7E60607C, 0x60606060 },
  // +
  { 0x0018187E, 0x18180000 },
  // !
  { 0x18181818, 0x00181800 }
};

int
main (int argc, char *argv[])
{
  const char *filename = "font.bin";
  if (argc > 1)
    {
      filename = argv[1];
    }

  int fd = open (filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1)
    {
      perror ("Error opening file");
      return EXIT_FAILURE;
    }

  if (bc_bigcharwrite (fd, (int *)BigChars, 18) == -1)
    {
      perror ("Error writing BigChars");
      close (fd);
      return EXIT_FAILURE;
    }

  close (fd);
  printf ("Font file '%s' created successfully\n", filename);
  return EXIT_SUCCESS;
}
