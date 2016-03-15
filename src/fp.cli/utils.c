/************************************************************************
*                                                                       *
* FileName:                                                             *
*            utils.c                                                    *
*                                                                       *
* Projecto:                                                             *
*            PR710_SDH_RA                                               *
*                                                                       *
* Descrição: Este módulo possui as funções misc                         *
*                                                                       *
*                                                                       *
* Histórico:                                                            *
*            3 Março 2003 - Primeira versao estavel                     *
*                                                                       *
*                                                                       *
* Autor: Celso Lemos                                                    *
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <net/if.h>
#include <sys/socket.h>
#include <linux/rtc.h>
#include <sys/times.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include "my_types.h"
//#include "controlo.h"
#include "utils.h"
//#include "fconfig.h"
#include <ctype.h>


/*********************************************************************************
* Função:         strToLong                                                      *
*                                                                                *
* Descrição:      Converte uma string num long                                   *
*                                                                                *
* Parametros:                                                                    *
*         pstr: string que se +pretende converter                                *
*         pValue: Variável onde vai ser armazenado o retorno                     *
*                                                                                *
* historico:                                                                     *
*         5 Dezembro de 2002                                                     *
*                                                                                *
* Autor: Paulo Gonçalves                                                         *
*********************************************************************************/
int StrToLong(char *pstr, uint32 *pValue)
{
  uint32 base,value;
  int signal,RetValue;

  if (strlen(pstr)==0) return(-1);
  signal=0;
  value=base=0;

  /* Deal with signal */
  if(*pstr=='-') signal=-1;
  else if(*pstr=='+') signal=+1;

  if (signal==0) signal=1;
  else pstr++;

  /* If first character is a zero (for base reference), skip it */
  if (*pstr=='0')
  {
    if      ((*(pstr+1)=='b') || (*(pstr+1)=='B')) base=2;
    else if ((*(pstr+1)=='o') || (*(pstr+1)=='O')) base=8;
    else if ((*(pstr+1)=='d') || (*(pstr+1)=='D')) base=10;
    else if ((*(pstr+1)=='h') || (*(pstr+1)=='H')) base=16;
    else if ((*(pstr+1)=='x') || (*(pstr+1)=='X')) base=16;
    /* Update pointer */
    if (base > 0)
      pstr += 2;
  }
  else
  {
    if      ((*(pstr+strlen(pstr)-1)=='b') || (*(pstr+strlen(pstr)-1)=='B')) base=2;
    else if ((*(pstr+strlen(pstr)-1)=='o') || (*(pstr+strlen(pstr)-1)=='O')) base=8;
    else if ((*(pstr+strlen(pstr)-1)=='d') || (*(pstr+strlen(pstr)-1)=='D')) base=10;
    else if ((*(pstr+strlen(pstr)-1)=='h') || (*(pstr+strlen(pstr)-1)=='H')) base=16;
    else if ((*(pstr+strlen(pstr)-1)=='x') || (*(pstr+strlen(pstr)-1)=='X')) base=16;
    /* Clear last character */
    if (base > 0)
      *(pstr+strlen(pstr)-1)='\0';
  }
  /* Default base is 10 */
  if (base==0) base=10;

  RetValue=(strlen(pstr)==0);
  while (*pstr && !RetValue){
    if ((*pstr>='0')&&(*pstr<='1')){
      value=value*base+(uint32)(*pstr-'0');
    }
    else if ((*pstr>='2')&&(*pstr<='7')){
      if (base<8) RetValue=-1;
      else value=value*base+(uint32)(*pstr-'0');
    }
    else if ((*pstr>='8')&&(*pstr<='9')){
      if (base<10) RetValue=-1;
      else value=value*base+(uint32)(*pstr-'0');
    }
    else if ((*pstr>='a')&&(*pstr<='f')){
      if (base<16) RetValue=-1;
      else value=value*base+(uint32)(*pstr-'a')+10;
    }
    else if ((*pstr>='A')&&(*pstr<='F')){
      if (base<16) RetValue=-1;
      else value=value*base+(uint32)(*pstr-'A')+10;
    }
    else RetValue=-1;
    pstr++;
  }
  if (!RetValue) *pValue = (signal==-1) ? ~(value-1) : value;

  return(RetValue);
}

int StrToLongLong(char *pstr, uint64 *pValue)
{
  uint64 base, value;
  int signal,RetValue;

  if (strlen(pstr)==0) return(-1);
  signal=0;
  value=base=0;

  /* Deal with signal */
  if(*pstr=='-') signal=-1;
  else if(*pstr=='+') signal=+1;

  if (signal==0) signal=1;
  else pstr++;

  /* If first character is a zero (for base reference), skip it */
  if (*pstr=='0')
  {
    if      ((*(pstr+1)=='b') || (*(pstr+1)=='B')) base=2;
    else if ((*(pstr+1)=='o') || (*(pstr+1)=='O')) base=8;
    else if ((*(pstr+1)=='d') || (*(pstr+1)=='D')) base=10;
    else if ((*(pstr+1)=='h') || (*(pstr+1)=='H')) base=16;
    else if ((*(pstr+1)=='x') || (*(pstr+1)=='X')) base=16;
    /* Update pointer */
    if (base > 0)
      pstr += 2;
  }
  else
  {
    if      ((*(pstr+strlen(pstr)-1)=='b') || (*(pstr+strlen(pstr)-1)=='B')) base=2;
    else if ((*(pstr+strlen(pstr)-1)=='o') || (*(pstr+strlen(pstr)-1)=='O')) base=8;
    else if ((*(pstr+strlen(pstr)-1)=='d') || (*(pstr+strlen(pstr)-1)=='D')) base=10;
    else if ((*(pstr+strlen(pstr)-1)=='h') || (*(pstr+strlen(pstr)-1)=='H')) base=16;
    else if ((*(pstr+strlen(pstr)-1)=='x') || (*(pstr+strlen(pstr)-1)=='X')) base=16;
    /* Clear last character */
    if (base > 0)
      *(pstr+strlen(pstr)-1)='\0';
  }
  /* Default base is 10 */
  if (base==0) base=10;

  RetValue=(strlen(pstr)==0);
  while (*pstr && !RetValue){
    if ((*pstr>='0')&&(*pstr<='1')){
      value=value*base+(uint64)(*pstr-'0');
    }
    else if ((*pstr>='2')&&(*pstr<='7')){
      if (base<8) RetValue=-1;
      else value=value*base+(uint64)(*pstr-'0');
    }
    else if ((*pstr>='8')&&(*pstr<='9')){
      if (base<10) RetValue=-1;
      else value=value*base+(uint64)(*pstr-'0');
    }
    else if ((*pstr>='a')&&(*pstr<='f')){
      if (base<16) RetValue=-1;
      else value=value*base+(uint64)(*pstr-'a')+10;
    }
    else if ((*pstr>='A')&&(*pstr<='F')){
      if (base<16) RetValue=-1;
      else value=value*base+(uint64)(*pstr-'A')+10;
    }
    else RetValue=-1;
    pstr++;
  }
  if (!RetValue) *pValue = (signal==-1) ? ~(value-1) : value;

  return(RetValue);
}

/*********************************************************************************
* Função:         strToDigit                                                     *
*                                                                                *
* Descrição:      Converte uma string num long                                   *
*                                                                                *
* Parametros:                                                                    *
*         pstr: string que se +pretende converter                                *
*         pValue: Variável onde vai ser armazenado o retorno                     *
*                                                                                *
* historico:                                                                     *
*         7 Maio de 2009                                                         *
*                                                                                *
* Autor: Ana Claudia Dias                                                        *
*********************************************************************************/

int strToBinary ( char * xi_string, uint8 * xo_buffer, uint32 xi_max_length )
{
    char * read ;
    uint8 * write ;
    uint8 byte ;

    /* Check string length */
    if ( strlen ( xi_string ) < xi_max_length * 2 )
    {
        /* String is too short */
        return ( -1 ) ;
    }

    /* Check string values */
    read = xi_string ;
    while ( ( * read ) && ( read - xi_string < strlen ( xi_string ) ) )
    {
        /* Convert to uppercase and make sure this is a hex digit */
        if ( ! isxdigit ( (int) * read ) )
        {
            /* Invalid string */
            return ( -2 ) ;
        }
        * read = toupper ( * read ) ;
        read ++ ;
    }


    /* Convert string */
    read = xi_string ;
    write = xo_buffer ;
    while ( * read && ( write - xo_buffer < xi_max_length ) )
    {
        byte = ( isdigit ( (int) * read ) ? * read - '0' : * read - 'A' + 0x0A ) ;
        byte <<= 4 ;
        read ++ ;
        byte += ( isdigit ( (int) * read ) ? * read - '0' : * read - 'A' + 0x0A ) ;
        read ++ ;
        * write ++ = byte ;
    }

    /* Return calculated length */
    return ( write - xo_buffer ) ;
}


int strSize ( char * xi_string)
{
    int i;

    /* Check string length */
    i = ( strlen ( xi_string ) );

//    printf(" strSIZE %d\n\r", i);
    return i;
}

//******************************************************************************
// Entrada: 
// pisspace   - Funcao que define separadores
// pisalnum   - Funcao que define um token composto
// ppline     - Ponteiro para a string (avanca para proximo ponto a analisar)
// 
// Saida:
// ppline     - Ponteiro para o resto da string
// ptoken     - Token encontrado
// 
// Retorno:
// -1 - String na entrada vazia
//  0 - Encontrou token (retornado em ptoken)
// Outros casos - Encontrou simbolo nao definido como separador nem caracter token
// 
//******************************************************************************

int
line_parser(int (*pisspace)(char c), int (*pisalnum)(char c), char **ppline, char *ptoken)
{
  int value;

  while( (*pisspace)(**ppline) ) (char*)(*ppline)++;
  
  if ( !(**ppline) ) {
    value = -1;
  }
  else if ( !(*pisalnum)(**ppline) ) {
    value = **ppline;
    *ptoken = **ppline;
    ptoken++;
    (char*)(*ppline)++;
  }
  else {
    for ( value = 0 ; (*pisalnum)(**ppline) ; value++ ) {
      if ( value < (TOKEN_MAX_SIZE-1) ) {
        *ptoken = **ppline;
        ptoken++;
      }
      (char*)(*ppline)++;
    }
    value = 0;
  }
  *ptoken=0;
  return value;
}

//******************************************************************************
//  Descricao:
//  Analisa e separa tokens numa linha de caracteres.
//  Ex: dia=23&mes=11&ano=2004
//
//  Entrada:
//  buffer    - Linha ou arquivo
//  token     - Variavel a procurar
//
//  Saida:
//  value     - Valor encontrado
//
//  Retorno
//   0 - Encontrou o token mas nao tem valor associado
//   1 - Encontrou o token e respectivo valor
//  -1 - Nao encontrou o token
//
//******************************************************************************

int
isspace_env( char c )
{
  return( ( c == ' ') || ( c == '\n') || ( c == '\r') || ( c == '\t') );
}

int
isalnum_env( char c )
{
  return(
    (( c >= 'a') && ( c <= 'z')) ||
    (( c >= 'A') && ( c <= 'Z')) ||
    (( c >= '0') && ( c <= '9')) ||
    ( c == '<') || ( c == '>') || ( c == '|') ||
    ( c == '(') || ( c == ')') || ( c == '[') || ( c == ']') || ( c == '*' ) || ( c == '%' ) ||
    ( c == '-') || ( c == '_') || ( c == '.') || ( c == ',') || ( c == ':' ) || ( c == ';' ) || ( c == '/' ) || ( c == '\\' )
  );
}

int
isalnum_env_arg( char c )
{
  return(
    (( c >= 'a') && ( c <= 'z')) ||
    (( c >= 'A') && ( c <= 'Z')) ||
    (( c >= '0') && ( c <= '9')) ||
    ( c == '<') || ( c == '>') || ( c == '|') ||
    ( c == '(') || ( c == ')') || ( c == '[') || ( c == ']') || ( c == '*' ) || ( c == '%' ) ||
    ( c == '-') || ( c == '_') || ( c == '.') || ( c == ',') || ( c == ':' ) || ( c == ';' ) || ( c == '/' ) || ( c == '\\' ) ||
    ( c == '=')
  );
}

//----

//******************************************************************************
// Converte string para long e valida limites
//******************************************************************************

int
str2value( char *str , long *pvalue , long min , long max )
{
  if ( StrToLong( str , (unsigned long*)pvalue) != 0 ) return(-1);
  if ( (*pvalue < min) || (*pvalue > max) ) return(-1);
  return 0;
}

//******************************************************************************
//******************************************************************************

