/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2014-2014
*
**********************************************************************
*
* @filename ptin_fpga_api.c
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


#include "l7_platformspecs.h"
#include "ptin_globaldefs.h"

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
* @comments 
*
* @end
*
*************************************************************************/
L7_uint8 ptin_fgpa_mx_is_active(void)
{
  return ((cpld_map->reg.mx_is_active == 0)? 0 :1 );
}
#endif//MAP_CPLD
      
#endif//PTIN_BOARD_IS_MATRIX
