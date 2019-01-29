//*****************************************************************************

#include <stdio.h>
#include "memchk.h"

//*****************************************************************************
//* Code
//*****************************************************************************

void memfill(char *start, unsigned int dim, char *with){
  char *pwith;

  if (!(*with)) return;
  pwith=with;
  while (dim--) {
    *start++ = *pwith++;
    if (!(*pwith)) pwith=with;
  }
}

//*****************************************************************************

int memchk(char *start, unsigned int dim, char *with){
  char *pwith;
  unsigned int i;

  if (!(*with)) return(-1);
  for (i=0, pwith=with; i<dim; i++) {
    if (*start++ != *pwith++) break;
    if (!(*pwith)) pwith=with;
  }
  return(i);
}

//*****************************************************************************

void printmem(char *start, unsigned int dim){
  unsigned int i;

  for (i=0; i<dim; i++) {
    if (!(i % 8)) {
      if (!(i % 16)) printf("\n\r%08Xh: ",i);
      else printf("  ");
    }
    printf("%02X ",*start);
    start++;
  }
  printf("\n\r");
}

//*****************************************************************************

