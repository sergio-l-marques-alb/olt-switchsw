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
         
_EXTERN_ int StrToLongLong(char *pstr, uint64 *pValue);

_EXTERN_ int strToBinary ( char * xi_string, uint8 * xo_buffer, uint32 xi_max_length );
         
_EXTERN_ int strSize ( char * xi_string);

_EXTERN_ int str2value( char *str , long *pvalue , long min , long max );
         
#undef _EXTERN_

/***********************************************************************/

#endif //_UTILS_H_


