/************************************************************************
*                                                                       *
* FileName:                                                             *
*            crc.h                                                      *
*                                                                       *
* Projecto:                                                             *
*            PR710_SDH_RA                                               *
*                                                                       *
* Descri��o: Header file para o modulo de software crc.c                *
*                                                                       *
*                                                                       *
* Hist�rico:                                                            *
*            22 Janeiro 2003 - primeira vers�o                          *
*                                                                       *
*                                                                       *
* Autor: Celso Lemos / Nuno Pires                                       *
************************************************************************/
#ifndef _CRC_H_
#define _CRC_H_

#include "globals.h"
#include <stdio.h>


/*********************************************************************************
* Fun��o:         getCRC16                                                       *
*                                                                                *
* Descri��o:      Retorna o crc de um determinado bloco de bytes coniguos        *
*                                                                                *
* Parametros:                                                                    *
*			  startaddr: endere�o do primeiro byte do bloco                      *
*             size:      numero de bytes do bloco                                *
*                                                                                *
* retorno:                                                                       *
*                Retorna uma word com o crc calculado                            *
*                                                                                *
* historico:                                                                     *
*         9 Julho de 2002                                                     *
*                                                                                *
* Autor: Nuno Pires                                                              *
*                                                                                *
*********************************************************************************/
#ifdef __cplusplus
extern "C"
#else
extern
#endif
uint16 getCRC16(uint8 *startaddr,uint32 size);

#ifdef __cplusplus
extern "C"
#else
extern
#endif
uint16 getCRC16_from_file(FILE *fp, uint32 size);

#ifdef __cplusplus
extern "C"
#else
extern
#endif
uint16 getCRC16_next(uint8 *startaddr,uint32 size,uint16 startcrc);

#endif //_CRC_H_
