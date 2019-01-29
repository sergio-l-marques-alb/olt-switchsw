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
#include "globals.h"
//#include "controlo.h"
#include "main.h"
#include "crc.h"
#include "i2c.h"
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

  if(*pstr=='-') signal=-1;
  else if(*pstr=='+') signal=+1;

  if      ((*(pstr+strlen(pstr)-1)=='b') || (*(pstr+strlen(pstr)-1)=='B')) base=2;
  else if ((*(pstr+strlen(pstr)-1)=='o') || (*(pstr+strlen(pstr)-1)=='O')) base=8;
  else if ((*(pstr+strlen(pstr)-1)=='d') || (*(pstr+strlen(pstr)-1)=='D')) base=10;
  else if ((*(pstr+strlen(pstr)-1)=='h') || (*(pstr+strlen(pstr)-1)=='H')) base=16;

  if (signal==0) signal=1;
  else pstr++;

  if (base==0) base=10;
  else *(pstr+strlen(pstr)-1)='\0';

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

  if(*pstr=='-') signal=-1;
  else if(*pstr=='+') signal=+1;

  if      ((*(pstr+strlen(pstr)-1)=='b') || (*(pstr+strlen(pstr)-1)=='B')) base=2;
  else if ((*(pstr+strlen(pstr)-1)=='o') || (*(pstr+strlen(pstr)-1)=='O')) base=8;
  else if ((*(pstr+strlen(pstr)-1)=='d') || (*(pstr+strlen(pstr)-1)=='D')) base=10;
  else if ((*(pstr+strlen(pstr)-1)=='h') || (*(pstr+strlen(pstr)-1)=='H')) base=16;

  if (signal==0) signal=1;
  else pstr++;

  if (base==0) base=10;
  else *(pstr+strlen(pstr)-1)='\0';

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
* Função:         calc_crc7                                                      *
*                                                                                *
* Descrição:      Calculo do crc7 - utilizar no TIM dos VCs                      *
*                                                                                *
* Parametros:                                                                    *
*	    		 input: ponteiro para o primeiro char                            *
*                                                                                *
* retorno:                                                                       *
*                Retorna uma word com o crc calculado                            *
*                                                                                *
* historico:                                                                     *
*         29 Abril de 2003                                                       *
*                                                                                *
* Autor: Celso Lemos                                                             *
*                                                                                *
*********************************************************************************/
uint8 calc_crc7(char *input)
{
uint8 aux,i,j,k,retvalue;
uint8 bit[9]={0,0,0,0,0,0,0,0};
uint8 bit_old[9]={0,0,0,0,0,0,0,0};

   input[0]=0x80;
   
   for (i=0;i<16;i++) {

       aux = input[i];
       for (j=0;j<8;j++) {

           bit[2] = ((aux>>(7-j)) & 1) ^ bit_old[8];
           bit[3] = bit_old[2];
           bit[4] = bit_old[3];
           bit[5] = bit_old[4] ^ bit[2];
           bit[6] = bit_old[5];
           bit[7] = bit_old[6];
           bit[8] = bit_old[7];

           for (k=0;k<9;k++) {
               bit_old[k] = bit[k];
           }
       }
   }

   retvalue=0x80;
   for (k=2;k<9;k++) {
     //printf("bit[%d]=0x%.2x\n\r",k,bit[k]);
     retvalue |= (bit[k]<<(k-2));
   }
   //printf("crc=0x%.2x\n\r",retvalue);
   return(retvalue);
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

int
read_env(char *buffer, char *token, char *value)
{
  char  var[TOKEN_MAX_SIZE];
  char  val[TOKEN_MAX_SIZE];
  int   result;

  value[0] = '\0';
  if ( buffer == NULL ) return(-1);

  while (-1) {
    result = line_parser( &isspace_env, &isalnum_env, &buffer, var);
    if ( result < 0 ) return(-1);
    else {
      if ( result == '&' ) continue;
      if ( result == 0 ) {
        result = line_parser( &isspace_env, &isalnum_env, &buffer , val);
        if ( result == '=' ) {
          result = line_parser( &isspace_env, &isalnum_env_arg, &buffer , val);
        }
        else {
          result = -1;
        }
        if ( strcmp( token , var) == 0) {
          if ( result >= 0 ) {
            strcpy( value , val);
            return(1);
          }
          else {
            return(0);
          }
        }
      }
    }
  }
}

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
// Procura token=value e converte value para int e valida limites
//******************************************************************************

int
read_env_token2value( char *buffer, char *token, int *value, int min , int max )
{
  char  svalue[TOKEN_MAX_SIZE];
  long  lvalue;

  switch( read_env(buffer, token, svalue ) ){
    case  1:
      if ( str2value( svalue , &lvalue , min , max ) == 0 ) {
        if ( *value != (int)lvalue ) {
          *value = (int)lvalue;
          return 0x1;
        }
        return 0x0;
      }
      return 0x2;
    case -1:
      return 0x0;
    case  0:
    default:
      return 0x2;
  }
}

//******************************************************************************
// RTC
//******************************************************************************

int
read_rtc( TRTC *rtc_tm )
{
  time_t  ttnow;

  ttnow = time(NULL);
  localtime_r(&ttnow, rtc_tm);
  return(0);
}

//------------------------------------------------------------------------------

int
write_rtc( TRTC *rtc_tm )
{
  time_t           ttnow;
  int              fd;
  struct rtc_time  rtc;

  // Sistema
  ttnow = mktime( rtc_tm );
  stime( &ttnow );

  // RTC
  if ( ( fd = open ("/dev/rtc", O_RDWR ) ) == -1 ) return -1;
  if ( ioctl( fd , RTC_RD_TIME , &rtc ) != 1 ) {
    rtc.tm_sec   = rtc_tm->tm_sec ;           
    rtc.tm_min   = rtc_tm->tm_min ;              
    rtc.tm_hour  = rtc_tm->tm_hour;
    rtc.tm_mday  = rtc_tm->tm_mday;
    rtc.tm_mon   = rtc_tm->tm_mon ;
    rtc.tm_year  = rtc_tm->tm_year;
    ioctl( fd , RTC_SET_TIME , &rtc );
  }
  close( fd );
  return(0);
}

//------------------------------------------------------------------------------

int
str_to_date( char *str , int ret )
{
  TRTC   aux_tm;
  int    value;

  if ( sscanf( str , "%4d/%2d/%2d", &(aux_tm.tm_year), &(aux_tm.tm_mon) , &(aux_tm.tm_mday) ) != 3 ) {
    if ( sscanf( str , "%4d-%2d-%2d", &(aux_tm.tm_year), &(aux_tm.tm_mon) , &(aux_tm.tm_mday) ) != 3 ) {
      return ret;
    }
  }

  aux_tm.tm_year -= 1900;
  aux_tm.tm_mon  -= 1;
  //aux_tm.tm_mday;
  aux_tm.tm_hour = 0;
  aux_tm.tm_min  = 0;
  aux_tm.tm_sec  = 0;

  //printf("amd=%d,%d,%d\n", aux_tm.tm_year , aux_tm.tm_mon , aux_tm.tm_mday );
  if ( ( value = mktime( &aux_tm ) ) > 0 ) {
    //printf("ok!\n");
    return value;
  }
  return ret;
}

//------------------------------------------------------------------------------

int
str_to_time( char *str , int ret )
{
  TRTC   aux_tm;
  int    value;

  if ( sscanf( str , "%2d:%2d:%2d", &(aux_tm.tm_hour), &(aux_tm.tm_min) , &(aux_tm.tm_sec) ) == 3 ) {
    aux_tm.tm_year = 80;
    aux_tm.tm_mon  = 0;
    aux_tm.tm_mday = 1;
    //aux_tm.tm_hour = 0;
    //aux_tm.tm_min  = 0;
    //aux_tm.tm_sec  = 0;

    //printf("hms=%d,%d,%d\n", aux_tm.tm_hour , aux_tm.tm_min , aux_tm.tm_sec );
    if ( ( value = mktime( &aux_tm ) ) > 0 ) {
      //printf("ok!\n");
      return value % 86400;
    }
  }
  return ret;
}

//------------------------------------------------------------------------------

int
write_rtc_str( char *str )
{
  TRTC  aux_tm;
  TRTC  rtc_tm;

  read_rtc( &rtc_tm );
  if ( sscanf( str , "%4d/%2d/%2d %2d:%2d:%2d",
                    &(aux_tm.tm_year), &(aux_tm.tm_mon) , &(aux_tm.tm_mday),
                    &(aux_tm.tm_hour), &(aux_tm.tm_min) , &(aux_tm.tm_sec) ) == 6 ) {
    rtc_tm.tm_year = aux_tm.tm_year - 1900;
    rtc_tm.tm_mon  = aux_tm.tm_mon - 1;
    rtc_tm.tm_mday = aux_tm.tm_mday;
    rtc_tm.tm_hour = aux_tm.tm_hour;
    rtc_tm.tm_min  = aux_tm.tm_min ;
    rtc_tm.tm_sec  = aux_tm.tm_sec ;
  }
  else if ( sscanf( str , "%2d:%2d:%2d",
               &(aux_tm.tm_hour), &(aux_tm.tm_min) , &(aux_tm.tm_sec) ) == 3 ) {
  
    rtc_tm.tm_hour = aux_tm.tm_hour;
    rtc_tm.tm_min  = aux_tm.tm_min ;
    rtc_tm.tm_sec  = aux_tm.tm_sec ;
  }
  else if ( sscanf( str , "%4d/%2d/%2d",
                    &(aux_tm.tm_year), &(aux_tm.tm_mon) , &(aux_tm.tm_mday) ) == 3 ) {
    rtc_tm.tm_year = aux_tm.tm_year - 1900;
    rtc_tm.tm_mon  = aux_tm.tm_mon - 1;
    rtc_tm.tm_mday = aux_tm.tm_mday;
  }
  else {
    return(-1);
  }
  write_rtc( &rtc_tm );
  return(0);
}

//******************************************************************************
//******************************************************************************

