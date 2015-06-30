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
L7_uint8 ptin_fpga_mx_is_matrixactive(void);

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
L7_uint8 ptin_fpga_mx_is_matrixactive_rt(void);

/**
 * Check if current Matrix is the Working one (slot 1) 
 * (Only for Matrix board)
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fpga_mx_is_matrix_in_workingslot(void);
#endif

#if (PTIN_BOARD_IS_LINECARD)
/**
 * Check if Active Matrix is in Working slot 
 * (Only for linecards)
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fpga_lc_is_matrixactive_in_workingslot(void);
#endif

#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)

typedef enum
{
  PTIN_FPGA_STANDBY_MATRIX=0,
  PTIN_FPGA_ACTIVE_MATRIX
} ptin_fpga_matrix_type_t;

/**
 * Get slot position of Active Matrix 
 * (For all cards) 
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fpga_matrixActive_slot(void);

/**
 * Get slot position of Inactive Matrix 
 * (For all cards) 
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fpga_matrixInactive_slot(void);

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
L7_uint8 ptin_fpga_mx_get_matrixactive(void);
#endif//(PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)

#endif//MAP_CPLD

/**
 * Get slot id 
 * (For all cards)
 * 
 * @return L7_uint8 : slot id
 */
L7_uint8 ptin_fpga_board_slot_get(void);

#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)
/**
 * Get active/backup matrix slot id.
 * 
 * @param matrixType : Matrix type (1-active; 0-backup) 
 * 
 * @return slotId  : Slot Id of the given  matrix type
 *  
 * @note When this method is used in a linecard, the matrixType parameter is ignored and the slotId returned always belongs to the active matrix 
 */
L7_uint8 ptin_fpga_matrix_slotid_get(ptin_fpga_matrix_type_t matrixType);

/**
 * Get active/backup matrix IP address.
 * 
 * @param matrixType : Matrix type (1-active; 0-backup)
 * @param 
 * 
 * @return ipAddr    : IP address of the given matrix type
 *  
 * @note When this method is used in a linecard, the matrixType parameter is ignored and the IP address returned always belongs to the active matrix 
 */
L7_uint32 ptin_fpga_matrix_ipaddr_get(ptin_fpga_matrix_type_t matrixType);

/**
 * Get slot IP address.
 * 
 * @param slotId [in] : Slot Id 
 * @param ipAddr [out]: IP Address  
 * 
 * @return rc    : L7_SUCESS 
 *                 L7_FAILURE 
 *  
 */
L7_RC_t ptin_fpga_slot_ip_addr_get(L7_uint8 slotId, L7_uint32 *ipAddr);

/**
 * Get Board Type
 * 
 * 
 * @return boardType    : PTIN_BOARD_CXO160G 
 *                        PTIN_BOARD_CXO640G 
 *  
 */
L7_uint32 ptin_fpga_board_get(void);
#endif//(PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)

#endif //_PTIN_FPGA_API_H
