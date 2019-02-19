#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <termios.h>
#include "osapi.h"

static void osapiSerialDevshellTask (void)
{
  #if (L7_SERIAL_COM_ATTR)
  struct termios term;
  #endif
  char cmd[80];
  int crPos;

  #if (L7_SERIAL_COM_ATTR)
  if (tcgetattr (fileno (stdin), &term) == -1)
  {
    perror ("tcgetattr");
    return;
  }
  system ("stty sane");
  #endif

  while (1)
  {
    printf ("SmartPATH Debug >");
    fflush (stdout);
    memset (cmd, 0, sizeof (cmd));
    read (STDIN_FILENO, cmd, sizeof (cmd) - 1);
    crPos = strlen (cmd);
    cmd[crPos - 1] = '\0';
    osapiDevShellExec (cmd);
  }
  #if (L7_SERIAL_COM_ATTR)
  if (tcsetattr (fileno (stdin), TCSAFLUSH, &term) == -1)
  {
    perror ("tcsetattr");
  }
  #endif
}

void osapiDevshellTaskInit (void)
{
  osapiTaskCreate ("serialInput", (void *) osapiSerialDevshellTask, 0, L7_NULLPTR, 16 * 1024, 5, 0);
}
