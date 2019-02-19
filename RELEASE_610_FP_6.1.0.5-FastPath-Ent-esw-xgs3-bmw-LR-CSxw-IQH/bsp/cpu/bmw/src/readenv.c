//*****************************************************************************

#include <stdio.h>
#include "addrmap.h"
#include "readenv.h"

//*****************************************************************************
//* Defs
//*****************************************************************************

#define BOOTENV_SIZE    (64*1024)
#define BOOTENV_NUM     1    // Sem redundancia

static unsigned int BOOTENV_BASE[BOOTENV_NUM] = {
  0xFFF40000
};

//*****************************************************************************
//* Vars
//*****************************************************************************

TAddrMap       flash;
volatile char *pflash;

//*****************************************************************************
//* Code
//*****************************************************************************

int
open_bootenv(void)
{
  int i;

  for ( i = 0 ; i < BOOTENV_NUM ; i++ ) {
    pflash = AddrAlloc( &flash, BOOTENV_BASE[i], BOOTENV_SIZE );
    if ( pflash != NULL ) {
      #if ( BOOTENV_NUM != 1 )
      if ( pflash[4] == 0x01 ) 
      #endif
        return( -1 );
      AddrFree( &flash );
    }
  }
  pflash = NULL;

  return(0);
}

//*****************************************************************************

int
close_bootenv(void)
{
  AddrFree(&flash);
  return(0);
}

//*****************************************************************************

int
read_bootenv(char *token, char *value)
{
  int i,j,k;
  char str[BOOTENV_SIZE];

  if ( pflash != NULL ) {
    i=4;
    j=k=0;
    while ( ( i < BOOTENV_SIZE ) && ( pflash[i-1] | pflash[i] ) ) {
      if (!pflash[i]) {
        if (j) {
          if ( j >= BOOTENV_MAX ) j = BOOTENV_MAX - 1;
          str[k]=str[j]=0;
          if (!strcmp( str , token ) ) {
            strcpy( value, &(str[k+1]) );
            return(0);
          }
          j=k=0;
        }
      }
      else {
        if ((!k) && (pflash[i]=='=')) k=j;
        str[j++] = pflash[i];
      }
      i++;
    }
  }
  value[0] = '\0';
  return(-1);
}

//*****************************************************************************

int
print_bootenv(void)
{
  int i,j;
  char str[BOOTENV_SIZE];

  if ( pflash != NULL ) {
    i=4;
    j=0;
    while ( ( i < BOOTENV_SIZE ) && ( pflash[i-1] | pflash[i] ) ) {
      if (!pflash[i]) {
        if (j) {
          str[j]=0;
          printf("[%s]\n\r",str);
          j=0;
        }
      }
      else {
        str[j++]=pflash[i];
      }
      i++;
    }
  }
  return(0);
}

//*****************************************************************************

