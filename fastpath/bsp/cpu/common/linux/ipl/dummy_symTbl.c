#include <stdio.h>

typedef struct
{
  char *str;
  void *func;
} SYMBOL;

SYMBOL standTbl[] = 
{
  {(char *)NULL, (void *)NULL},
};

unsigned long standTblSize = 1;
