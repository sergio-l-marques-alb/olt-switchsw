/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2014-2014
*
**********************************************************************
*
* @filename ptin_fpga_api.h
*
* @purpose API to get the status of the CPLD  register on the FPGA
*
* @component fpga
*
* @comments none
* 
* @create    06/06/2014
*
* @author    Marcio Daniel Melo
* 
* @end
**********************************************************************/
#ifndef _PTIN_FPGA_API_H
#define _PTIN_FPGA_API_H

#include "datatypes.h"

#if PTIN_BOARD_IS_MATRIX

#ifdef MAP_CPLD

/**************************************************************************
*
* @purpose  Verify if this Matrix is Currently in Active State. 
*
* @param    void
*
* @returns  TRUE or FALSE
*
* @comments If this Card is not a Matrix it is returned 0.
*
* @end
*
*************************************************************************/
L7_uint8 ptin_fgpa_mx_is_active(void);

#endif//MAP_CPLD

#endif//PTIN_BOARD_IS_MATRIX

#endif //_PTIN_FPGA_API_H
