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

#include "l7_platformspecs.h"
#include "ptin_globaldefs.h"

#ifdef MAP_CPLD

#if (PTIN_BOARD_IS_MATRIX)
/**************************************************************************
*
* @purpose  Verify if this Matrix is Currently in Active State. 
*           (Only for Matrix board)
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
L7_uint8 ptin_fgpa_mx_is_matrixactive(void);

/**************************************************************************
*
* @purpose  Verify if this Matrix is Currently in Active State.
*           (Only for Matrix board)
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
L7_uint8 ptin_fgpa_mx_is_matrixactive_rt(void);

/**************************************************************************
*
* @purpose  Verify if this Matrix is Currently in Active State.
*           (Only for Matrix board)
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
L7_uint8 ptin_fgpa_mx_get_matrixactive(void);

/**
 * Check if current Matrix is the Working one (slot 1) 
 * (Only for Matrix board)
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fgpa_mx_is_matrix_in_workingslot(void);
#endif

#if (PTIN_BOARD_IS_LINECARD)
/**
 * Check if Active Matrix is in Working slot 
 * (Only for linecards)
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fgpa_lc_is_matrixactive_in_workingslot(void);
#endif

#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)
/**
 * Get slot position of Active Matrix 
 * (For all cards) 
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fgpa_matrixActive_slot(void);

/**
 * Get slot position of Inactive Matrix 
 * (For all cards) 
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fgpa_matrixInactive_slot(void);
#endif

#endif//MAP_CPLD

/**
 * Get slot id 
 * (For all cards)
 * 
 * @return L7_uint8 : slot id
 */
L7_uint8 ptin_fgpa_board_slot(void);

#endif //_PTIN_FPGA_API_H
