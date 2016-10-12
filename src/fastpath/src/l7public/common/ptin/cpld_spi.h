/********************************************************************
 *            FPGA : FPGA1255                                       *
 *          Versao : 1                                              *
 *           Autor : PTIN\x00563                                    *
 *         Maquina : WITHUS-BUGS-2-U                                *
 *            Data : 13-08-2012 18:13:07                            *
 *       FPGAsCtrl : V3.2.3                                         *
 ********************************************************************/
#ifndef __CPLD_SPI_H__
#define __CPLD_SPI_H__

#include "ptin_globaldefs.h"

#if (PTIN_BOARD == PTIN_BOARD_TG16GF || PTIN_BOARD == PTIN_BOARD_OLT1T0F  || PTIN_BOARD == PTIN_BOARD_TT04SXG /*|| PTIN_BOARD == PTIN_BOARD_OLT1T0*/)

extern int cpld_spi_create(void);

extern int cpld_spi_destroy(void);

extern unsigned char cpld_spi_read(unsigned int addr);

extern unsigned char cpld_spi_write(unsigned int addr, unsigned char val);

#endif /* PTIN_BOARD == PTIN_BOARD_TG16GF || PTIN_BOARD == PTIN_BOARD_OLT1T0F || PTIN_BOARD == PTIN_BOARD_TT04SXG */

#endif // __CPLD_SPI_H__
