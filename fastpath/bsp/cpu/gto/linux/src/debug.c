#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define _DEBUG_C_
#include "debug.h"
#undef _DEBUG_C_

//******************************************************************************
// Variaveis globais locais
//******************************************************************************

static int _debug_level_ = 0;

static FILE *_degug_output_ = NULL;

//******************************************************************************
// DB de memoria
//   level    : Mascara debug
//   ident    : String no inicio de cada linha
//   col      : Numero de colunas
//   buf      : Ponteiro para a memoria a listar
//   n        : Numero de bytes
//******************************************************************************

int
debug_print_dump( unsigned int level , char *ident , int col , char *buf , int n )
{
  int  i,j;
  char aux[1024];

  if ( level & _debug_level_ ) {
    if ( ( col < 1 ) || ( col > 128 ) || ( n < 0 ) || ( n > 0x8000 ) ) return -1;
    for ( i = 0 ; i < 0x8000 ; i+=col ) {
      if ( i >= n ) break;
      sprintf( aux , "%s%04X:" , ident , i );
      for ( j = 0 ; j < col ; j++ ) {
        if ( ( i + j ) < n ) sprintf( aux , "%s %02X" , aux , buf[i + j] );
        else strcat( aux , "   " );
      }
      strcat( aux , " \"" );
      for ( j = 0 ; j < col ; j++ ) {
        if ( ( i + j ) < n ) {
          if ( ( buf[i + j] >= 32 ) && ( buf[i + j] <= 127 ) ) sprintf( aux , "%s%c" , aux , buf[i + j] );
          else strcat( aux , "." );
        }
      }
      strcat( aux , "\"\n\r" );
      printf_debug( level , "%s", aux); 
    }
  }
  return 0;
}

//******************************************************************************
// Help em string
//******************************************************************************

char *
debug_print_help( char *str , int ident )
{
  int    i;
  char   sident[24];

  if ( ident >= 24 ) ident = 23;
  memset( sident , ' ' , ident );
  sident[ident] = '\0';

  for ( i = 0 ; i < DEBUG_HELP_TAB_NUM ; i++ ) {
    sprintf(str,"%s%s%8X : %c%s\r\n", str, sident , debug_help_tab[i].level , ( debug_help_tab[i].level & _debug_level_ ) ? '+' : ' ' , debug_help_tab[i].str);
  }
  return( str );
}

//******************************************************************************
// Alterar mascara de debug
//******************************************************************************

int
debug_setlevel(int level)
{
  _debug_level_ = level;
  return(0);
}

int
debug_getlevel( void )
{
  return(_debug_level_);
}

//******************************************************************************
// Retorna !0 se debug activo
//******************************************************************************

int
debug_true(int level)
{
  return( level & _debug_level_ );
}

//******************************************************************************
// Print se debug activo 
//******************************************************************************

int
printf_debug( int level , const char *fmt,... )
{
  va_list ap;

  if ( ( _degug_output_ != NULL ) && ( ( level & _debug_level_ ) != 0 ) ) {
    va_start( ap , fmt );
    vfprintf( _degug_output_ , fmt , ap );
    va_end( ap );
    fflush( _degug_output_ );  // flush everything
    return(0);   
  }
  return(0);
}

//******************************************************************************
// Direcciona o output do debug
// Se path for NULL fecha o debug
//******************************************************************************

int
debug_setoutput( char *path )
{
  FILE *stream;

  if ( path == NULL ) {
    if ( _degug_output_ != NULL ) {
      fclose( _degug_output_ );
      _degug_output_ = NULL;
    }
    return(0);
  }
  if (( stream  = fopen( path , "wt")) != NULL ) {
    _degug_output_ = stream;
    return(0);
  }
  return(-1);
}

//******************************************************************************



