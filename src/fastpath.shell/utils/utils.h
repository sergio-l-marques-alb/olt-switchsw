/************************************************************************
*                                                                       *
* FileName:                                                             *
*            utils.h                                                    *
*                                                                       *
* Projecto:                                                             *
*            PR710_SDH_RA                                               *
*                                                                       *
* Descrição: Header file para o modulo utils.c                          *
*                                                                       *
*                                                                       *
* Histórico:                                                            *
*            3 Março 2003 - Primeira versao estavel                     *
*                                                                       *
*                                                                       *
* Autor: Celso Lemos                                                    *
************************************************************************/
#ifndef _UTILS_H_
#define _UTILS_H_

#define ENV_EQUAL        '='
#define TOKEN_MAX_SIZE   256

typedef struct tm TRTC;

/***********************************************************************/
#ifdef __cplusplus
#define _EXTERN_ extern "C"
#else
#define _EXTERN_ extern
#endif


_EXTERN_ int StrToLong(char *pstr, uint32 *pValue);
         
_EXTERN_ uint8 calc_crc7(char *input);
         
_EXTERN_ int strToBinary ( char * xi_string, uint8 * xo_buffer, uint32 xi_max_length );
         
_EXTERN_ int strSize ( char * xi_string);

_EXTERN_ int read_env(char *buffer, char *token, char *value);
         
_EXTERN_ int str2value( char *str , long *pvalue , long min , long max );
         
_EXTERN_ int read_env_token2value( char *buffer, char *token, int *value, int min , int max );

_EXTERN_ int read_rtc(TRTC *rtc_tm);

_EXTERN_ int write_rtc(TRTC *rtc_tm);

_EXTERN_ int write_rtc_str( char *str );

_EXTERN_ int str_to_date( char *str , int ret );

_EXTERN_ int str_to_time( char *str , int ret );

#undef _EXTERN_

/***********************************************************************/

#endif //_UTILS_H_


